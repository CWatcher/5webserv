
#include "Server.hpp"
#include "socket/ListenSocket.hpp"
#include "socket/SessionSocket.hpp"
#include "handlers/base/HandlerTask.hpp"
#include "handlers/runner/runner.hpp"
#include "utils/syntax.hpp"

#include "SimpleHandler.hpp"

int Server::poll_timeout = 30 * 1000;

Server::Server(const char *filename): _config(filename)
{
    std::map<in_addr_t, std::set<in_port_t> > listened = _config.getListened();

    for(std::map<in_addr_t, std::set<in_port_t> >::const_iterator it = listened.begin();
        it != listened.end(); ++it)
    {
        in_addr_t ip   = it->first;
        cforeach(std::set<in_port_t>, it->second, port)
        {
            try
            {
                ASocket  *socket_listen = new ListenSocket(ip, *port);
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

    std::clog << _config;
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
            if (new_events == -1ul)
                logger::error << "Server: poll: " << logger::cerror << logger::end;
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
        delete _sockets[poll_fd->fd];
        _sockets.erase(poll_fd->fd);
    }

    return (poll_fd->events == poll_fd->revents);
}

void Server::eventAction(ASocket *socket)
{
    int return_value = socket->action();

    if (return_value != -1)
        try
        {
            _sockets[return_value] = new SessionSocket(return_value, socket->ip(), socket->port());
        }
        catch(const std::bad_alloc& e)
        {
            logger::error << "Server: unable add new Session: " << e.what() << logger::end;
        }
    else if (socket->state() == SocketState::Disconnect)
    {
        _sockets.erase(socket->fd());
        delete socket;
    }
    else if (socket->state() == SocketState::Process)
        addProcessTask(socket);
}

void Server::addProcessTask(ASocket *socket)
{
    SessionSocket       *session = static_cast<SessionSocket *>(socket);
    HTTPRequest         &request = session->request();
    const VirtualServer &v_server = _config.getVirtualServer(session->ip(), session->port(), request.getHeaderHostName());
    const Location      &location = VirtualServer::getLocation(v_server, request.uri());
    SimpleHandler       handler(location, session->request());

    logger::debug << "I know you came from port: " << ntohs(session->port()) << logger::end;
    logger::debug << "Your server config:\n" << location << logger::end;
    handler.fillResponse(session->response());
    session->setStateToWrite();

    // try
    // {
    //     HandlerTask*    new_task = new HandlerTask(VirtualServer::getLocation(v_server, request.uri()), session);
    //     _thread_pool.push_task(handlers::run, new_task);
    // }
    // catch(const std::bad_alloc& e)
    // {
    //     logger::error << "Server: unable add new Task" << e.what() << logger::end;
    //     return ;
    // }
    // logger::info << "Server: addProcessTask: sent to Task queue, socket " << session->fd() << logger::end;
}
