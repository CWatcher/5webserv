
#include "Server.hpp"
#include "socket/SocketListen.hpp"
#include "HTTPMessage.hpp"
#include "handlers/base/HandlerTask.hpp"
#include "handlers/runner/runner.hpp"
#include "utils/syntax.hpp"

int Server::poll_timeout = 30 * 1000;

Server::Server(ServerConfig &config)
    : _config(config)
{
    std::map<in_addr_t, std::set<in_port_t> > listened = config.getListened();

    for(std::map<in_addr_t, std::set<in_port_t> >::const_iterator it = listened.begin();
        it != listened.end(); ++it)
    {
        in_addr_t ip   = it->first;
        cforeach(std::set<in_port_t>, it->second, port)
        {
            try
            {
                ASocket  *socket_listen = new SocketListen(ip, *port);
                _sockets[socket_listen->fd()] = socket_listen;
            }
            catch (const std::exception &e)
            {
                for (std::map<int, ASocket *>::const_iterator it = _sockets.begin(); it != _sockets.end(); ++it)
                    delete it->second;
                throw;
            }
        }
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
    std::vector<pollfd> poll_array;
    size_t	            poll_array_len;
    size_t              new_events;

    logger::info << "Server: entering main loop..." << logger::end;

    while (!_sockets.empty()) // не запускаем, если нет слушающих сокетов?
    {
        // Обернурть всё тело цикла в try, чтобы никогда не выходить из него?
        poll_array_len = eventArrayPrepare(poll_array);

        logger::debug << "Server: polling..." << logger::end;
        // что будет если poll_array_len == 0, ок?
        new_events = poll(&poll_array[0], poll_array_len, poll_timeout);
        logger::debug << "Server: polling done!" << logger::end;

        if (new_events <= 0)
        {
            if (errno)
            {
                logger::error << "Server: " << logger::cerror << logger::end;
                errno = 0;
            }
            else
                logger::debug << "Server: no new events. Reached poll timeout (seconds): "
                              << poll_timeout / 1000 << logger::end;
            continue ;
        }

        for (size_t i = 0; i < poll_array.size(); ++i)
            if (eventCheck(&poll_array[i]))
            {
                ASocket *socket = _sockets[poll_array[i].fd];
                socket->action(this);
            }
    }
}

size_t Server::eventArrayPrepare(std::vector<pollfd> &poll_array) const
{
    size_t	poll_array_len = 0;

    try {
        poll_array.resize(_sockets.size());
    }
    catch (std::bad_alloc &e) {
        logger::error << "Server: eventArrayPrepare: " << e.what() << logger::end;
        return 0;
    }

    for (std::map<int, ASocket *>::const_iterator it = _sockets.begin(); it != _sockets.end(); ++it)
    {
        ASocket	*socket = it->second;

        if (socket->state() == SocketState::Read)
            poll_array[poll_array_len].events = POLLIN;
        else if (socket->state() == SocketState::Write)
            poll_array[poll_array_len].events = POLLOUT;
        else
            continue ;

        poll_array[poll_array_len].fd = socket->fd();

        ++poll_array_len;
    }
    return poll_array_len;
}

bool Server::eventCheck(const pollfd *poll_fd)
{
    if (poll_fd->revents & (POLLERR | POLLHUP | POLLNVAL))
    {
        logger::warning << "Server: got terminating event on socket " << poll_fd->fd << logger::end;
        deleteSession(poll_fd->fd);
    }

    return (poll_fd->events == poll_fd->revents);
}

void Server::addSession(int fd, in_addr_t from_listen_ip, in_port_t from_listen_port)
{
    try
    {
        _sockets[fd] = new SocketSession(fd, from_listen_ip, from_listen_port);
    }
    catch(const std::bad_alloc& e)
    {
        logger::error << "Server: addSocketSession: " << e.what() << logger::end;
    }
}

void Server::deleteSession(int fd)
{
    delete _sockets[fd];
    _sockets.erase(fd);
}

void Server::addProcessTask(SocketSession *session)
{
    const std::string   &server_name = session->input.getHeaderHostName();
    const VirtualServer &config = _config.getVirtualServer(session->ip(), session->port(), server_name);

    _thread_pool.push_task(handlers::run, new HandlerTask(config, session));
    logger::info << "Server: addProcessTask: sent to Task queue, socket " << session->fd() << logger::end;
}
