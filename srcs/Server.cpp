
#include "Server.hpp"
#include "HTTPMessage.hpp"
#include "SocketSession.hpp"

#include <cerrno>

const char      *DefaultResponse =	  "HTTP/1.1 200 OK\n"
                                      "Date: Wed, 18 Feb 2021 11:20:59 GMT\n"
                                      "Server: Apache\n"
                                      "X-Powered-By: webserv\n"
                                      "Last-Modified: Wed, 11 Feb 2009 11:20:59 GMT\n"
                                      "Content-Type: text/html; charset=utf-8\n"
                                      "Content-Length: 14\n"
                                      "Connection: close\n"
                                      "\n"
                                      "<h1>HELLO WORLD</h1>\n";

int Server::poll_timeout = 30 * 1000;

Server::Server(const std::map<int, ASocket *> &sockets_to_listen)
    : _sockets(sockets_to_listen) {}

Server::~Server()
{
    log::info("Shutting down Server...");

    for (std::map<int, ASocket *>::const_iterator it = _sockets.begin(); it != _sockets.end(); ++it)
        delete it->second;

    _sockets.clear();

    log::info("Bye!");
}

void Server::mainLoopRun()
{
    std::vector<pollfd>	poll_array;
    size_t				poll_array_len;
    int					new_events;

    log::info("Entering main Server loop...");

    while (!_sockets.empty())
    {
        poll_array_len = eventArrayPrepare(poll_array);

        new_events = poll(&poll_array[0], poll_array_len, poll_timeout);

        if (new_events <= 0)
        {
            if (errno)
                log::cerrno();
            else
                log::debug("No new events. Reached poll timeout (seconds):", poll_timeout / 1000);
            continue ;
        }

        for (size_t i = 0; i < poll_array_len; ++i)
            if (eventCheck(&poll_array[i]))
                eventAction(_sockets[poll_array[i].fd]);
    }
}

size_t Server::eventArrayPrepare(std::vector<pollfd> &poll_array)
{
    size_t	poll_array_len = 0;

    try {
        poll_array.reserve(_sockets.size());
    }
    catch (std::bad_alloc &e) {
        log::error(e.what());
    }

    for (std::map<int, ASocket *>::const_iterator it = _sockets.begin(); it != _sockets.end(); ++it)
    {
        ASocket	*socket = it->second;

        if (poll_array_len >= poll_array.capacity())
        {
            log::warning("Not enough memory to add socket", socket->fd);
            _sockets.erase(socket->fd);
            delete socket;
            continue ;
        }

        if (socket->getTrigger() == TriggerEvent::Read)
            poll_array[poll_array_len].events = POLLIN;
        else if (socket->getTrigger() == TriggerEvent::Write)
            poll_array[poll_array_len].events = POLLOUT;
        else
            continue ;

        poll_array[poll_array_len].fd = socket->fd;

        ++poll_array_len;
    }
    return poll_array_len;
}

bool Server::eventCheck(const pollfd *poll_fd)
{
    if (poll_fd->revents & (POLLERR | POLLHUP | POLLNVAL))
    {
        log::info("Got terminating event on socket", poll_fd->fd);
        delete _sockets[poll_fd->fd];
        _sockets.erase(poll_fd->fd);
    }

    return (poll_fd->events == poll_fd->revents);
}

void Server::eventAction(ASocket *socket)
{
    enum ASocket::PostAction	post_action;
    int		        		    action_value;

    action_value = socket->action(post_action);
    if (action_value == -1)
        log::cerrno();

    HTTPMessage DUMMY_RESPONSE; // TODO: delete after response implementation
    DUMMY_RESPONSE.raw_data = DefaultResponse;

    switch (post_action)
    {
        case ASocket::Add:
            _sockets[action_value] = new SocketSession(action_value);
            break ;

        case ASocket::Process:
            log::info("Sent to process queue socket", socket->fd);
            // put to real Process queue later

            log::info("HTTP response is ready for socket", socket->fd);
            dynamic_cast<SocketSession *>(socket)->prepareForWrite(DUMMY_RESPONSE);
            break ;

        case ASocket::Disconnect:
            _sockets.erase(socket->fd);
            delete socket;
            break ;

        default:
            break ;
    }
}

Server::Server() {}
Server::Server(const Server &_) {(void)_;}
Server &Server::operator=(const Server &_) {(void)_; return *this;}
