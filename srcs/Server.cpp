
#include "HTTPMessage.hpp"
#include "Server.hpp"
#include "SocketSession.hpp"
#include "handlers/runner/runner.hpp"
#include "utils/syntax.hpp"
#include "SocketListen.hpp"

#include <cerrno>
#include <cstring>

int Server::poll_timeout = 30 * 1000;

Server::Server(const std::vector<ServerConfig> &configs)
{
    cforeach(std::vector<ServerConfig>, configs, server)
    {
        std::map<in_addr_t, std::set<in_port_t> > listens_to = server->listen;

        for(std::map<in_addr_t, std::set<in_port_t> >::const_iterator listen = listens_to.begin();
            listen != listens_to.end(); listen++)
        {
            in_addr_t           ip = listen->first;
            std::set<in_port_t> ports = listen->second;

            cforeach(std::set<in_port_t>, ports, port)
            {
                std::pair<in_addr_t, in_port_t> address_pair = std::make_pair(ip, *port);

                if (server->server_name.empty())
                {
                    _config_by_address[address_pair][""] = *server;
                    continue;
                }
                cforeach(std::vector<std::string>, server->server_name, name)
                    _config_by_address[address_pair][*name] = *server;
            }
        }
    }

    for(std::map<std::pair<in_addr_t, in_port_t>, std::map<std::string, ServerConfig> >::const_iterator it = _config_by_address.begin();
        it != _config_by_address.end(); ++it)
    {
        ASocket  *socket_listen;
        in_addr_t ip   = it->first.first;
        in_port_t port = it->first.second;

        socket_listen = new SocketListen(ip, port);
        _sockets[socket_listen->fd] = socket_listen;
    }
}

Server::~Server()
{
    logger::info << "Server: shutting down..." << logger::end;

    for (std::map<int, ASocket *>::const_iterator it = _sockets.begin(); it != _sockets.end(); ++it)
        delete it->second;

    logger::info << "Server: bye!" << logger::end;
}

void Server::mainLoopRun()
{
    std::vector<pollfd>	poll_array;
    size_t				poll_array_len;
    int					new_events;

    logger::info << "Server: entering main loop..." << logger::end;

    while (!_sockets.empty())
    {
        poll_array_len = eventArrayPrepare(poll_array);

        logger::debug << "Server: polling..." << logger::end;
        new_events = poll(&poll_array[0], poll_array_len, poll_timeout);
        logger::debug << "Server: polling done!" << logger::end;

        if (new_events <= 0)
        {
            if (errno)
            {
                logger::error << "Server: " << strerror(errno) << logger::end;
                errno = 0;
            }
            else
                logger::debug << "Server: no new events. Reached poll timeout (seconds): "
                              << poll_timeout / 1000 << logger::end;
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
        logger::error << "Server: eventArrayPrepare: " << e.what() << logger::end;
    }

    for (std::map<int, ASocket *>::const_iterator it = _sockets.begin(); it != _sockets.end(); ++it)
    {
        ASocket	*socket = it->second;

        if (poll_array_len >= poll_array.capacity())
        {
            logger::warning << "Server: not enough memory to add socket " << socket->fd << logger::end;
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
        logger::warning << "Server: got terminating event on socket " << poll_fd->fd << logger::end;
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
    {
        logger::error << "Server: eventAction: " << strerror(errno) << socket->fd << logger::end;
        errno = 0;
    }

    switch (post_action)
    {
        case ASocket::Add:
            _sockets[return_value] = new SocketSession(
                return_value,
                reinterpret_cast<SocketListen *>(socket)->ip,
                reinterpret_cast<SocketListen *>(socket)->port
            );
            break ;

        case ASocket::Process:
            _thread_pool.push_task(handlers::run, socket);
            logger::info << "Server: eventAction: sent to Task queue, socket " << socket->fd << logger::end;
            break ;

        case ASocket::Disconnect:
            _sockets.erase(socket->fd);
            delete socket;
            break ;

        default:
            break ;
    }
}
