#include "sockets/SessionSocket.hpp"
#include "utils/log.hpp"

#include <sys/socket.h>

SessionSocket::SessionSocket(int fd, in_addr_t from_listen_ip, in_port_t from_listen_port, const in_addr &remote_addr) :
    ASocket(fd, from_listen_ip, from_listen_port),
    _remote_addr(remote_addr),
    _handler(NULL)
{}

int     SessionSocket::action(in_addr &)
{
    if (_state == SocketState::Read)
        read();
    else
        write();
    return -1;
}

void    SessionSocket::read()
{
    logger::debug << "Trying to read from socket " << _fd << logger::end;

    try
    {
        if (_request.read(_fd))
        {
            _state = SocketState::Handle;
            logger::debug << "HTTPRequest received on socket " << _fd << logger::end;
        }
    }
    catch (const std::runtime_error& re)
    {
        _state = SocketState::Disconnect;
        logger::error << re.what();
    }
    catch (const std::exception& e)
    {
        _state = SocketState::Disconnect;
    }
}

void    SessionSocket::write()
{
    if (!_response.isReady())
    {
        _handler->handle(_response);
        return;
    }

    logger::debug << "Trying to write to fd " << _fd << logger::end;
    try
    {
        if (_response.send(_fd))
        {
            if (_request.getHeaderValue("Connection") == "close")
                _state = SocketState::Disconnect;
            else
                _state = SocketState::Read;
            _request = HTTPRequest();
            _response = HTTPResponse();
            delete _handler;
            _handler = NULL;
            logger::debug << "HTTPResponse sent on socket " << _fd << logger::end;
        }
    }
    catch(const std::exception& e)
    {
        _state = SocketState::Disconnect;
    }
}
