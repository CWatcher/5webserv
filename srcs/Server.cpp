#include "Server.hpp"
#include "sockets/ListenSocket.hpp"
#include "handlers/AHandler.hpp"
#include "handlers/DeleteHandler.hpp"
#include "handlers/GetHandler.hpp"
#include "handlers/PostHandler.hpp"
#include "handlers/UndefinedHandler.hpp"
#include "utils/log.hpp"

#include <signal.h>

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

static bool server_on = true;

static void stop_server(int)
{
    server_on = false;
}

void Server::mainLoopRun()
{
    std::vector<pollfd> poll_array;
    size_t	            poll_array_len;
    size_t              new_events;

    logger::info << "Server: entering main loop..." << logger::end;
    ::signal(SIGINT, stop_server);
    while (server_on)
    {
        poll_array_len = eventArrayPrepare(poll_array);

        logger::debug << "Server: polling..." << logger::end;
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
    in_addr remote_addr;
    int     return_value = socket->action(remote_addr);

    if (return_value != -1)
        try
        {
            _sockets[return_value] = new SessionSocket(return_value, socket->ip(), socket->port(), remote_addr);
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
    else if (socket->state() == SocketState::Handle)
        handleRequest(socket);
}

void Server::handleRequest(ASocket *socket)
{
    SessionSocket       *session = static_cast<SessionSocket *>(socket);
    const VirtualServer &v_server = _config.getVirtualServer(session->ip(), session->port(), session->request().getHeaderHostName());
    const Location      &location = VirtualServer::getLocation(v_server, session->request().uri());
    AHandler            *handler;

    try
    {
        handler = getHandler(location, session);
        handler->makeResponse(session->response());
        delete handler;
        session->setStateToWrite();
    }
    catch (std::bad_alloc& ba)
    {
        logger::error << "Server: unable create handler: " << ba.what() << logger::end;
        _sockets.erase(session->fd());
        delete socket;
    }

    // logger::debug << "I know you came from port: " << ntohs(session->port()) << logger::end;
    // logger::debug << "Your server config:\n" << location << logger::end;
    // handler.makeResponse(session->response());

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

AHandler*   Server::getHandler(const Location &location, SessionSocket* session) const
{
    HTTPRequest&    request = session->request();

    if (request.method() == "DELETE")
        return new DeleteHandler(location, request);
    else if (request.method() == "GET" || request.method() == "HEAD")
        return new GetHandler(location, request, session->ip(), session->port(), session->remoteAddr());
    else if (request.method() == "POST")
        return new PostHandler(location, request, session->ip(), session->port(), session->remoteAddr());
    return new UndefinedHandler(location, request);
}
