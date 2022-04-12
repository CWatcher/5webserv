
#include "HTTPMessage.hpp"
#include "Server.hpp"
#include "SocketSession.hpp"

#include <cerrno>

int Server::poll_timeout = 30 * 1000;

Server::Server(const std::map<int, ASocket *> &sockets_to_listen)
    : _sockets(sockets_to_listen) {}

Server::~Server()
{
    logger::info("Shutting down Server...");

    for (std::map<int, ASocket *>::const_iterator it = _sockets.begin(); it != _sockets.end(); ++it)
        delete it->second;

    logger::info("Bye!");
}

void Server::mainLoopRun()
{
    std::vector<pollfd>	poll_array;
    size_t				poll_array_len;
    int					new_events;

    logger::info("Entering main Server loop...");

    while (!_sockets.empty())
    {
        poll_array_len = eventArrayPrepare(poll_array);

        logger::debug("Polling...");
        new_events = poll(&poll_array[0], poll_array_len, poll_timeout);
        logger::debug("Polling done!");

        if (new_events <= 0)
        {
            if (errno)
                logger::cerrno();
            else
                logger::debug("No new events. Reached poll timeout (seconds):", poll_timeout / 1000);
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
        logger::error(e.what());
    }

    for (std::map<int, ASocket *>::const_iterator it = _sockets.begin(); it != _sockets.end(); ++it)
    {
        ASocket	*socket = it->second;

        if (poll_array_len >= poll_array.capacity())
        {
            logger::warning("Not enough memory to add socket", socket->fd);
            _sockets.erase(socket->fd);
            delete socket;
            continue ;
        }

        if (socket->getTrigger() == TriggerType::Read)
            poll_array[poll_array_len].events = POLLIN;
        else if (socket->getTrigger() == TriggerType::Write)
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
        logger::info("Got terminating event on socket", poll_fd->fd);
        delete _sockets[poll_fd->fd];
        _sockets.erase(poll_fd->fd);
    }

    return (poll_fd->events == poll_fd->revents);
}

void Server::eventAction(ASocket *socket)
{
    enum ASocket::PostAction    post_action;
    int                         return_value;

    return_value = socket->action(post_action);
    if (return_value == -1)
        logger::cerrno(socket->fd);

    switch (post_action)
    {
        case ASocket::Add:
            _sockets[return_value] = new SocketSession(return_value);
            break ;

        case ASocket::Process:
            _thread_pool.pushTaskToQueue(reinterpret_cast<SocketSession *>(socket));
            logger::info("Sent to Task queue, socket", socket->fd);
            break ;

        case ASocket::Disconnect:
            _sockets.erase(socket->fd);
            delete socket;
            break ;

        default:
            break ;
    }
}
