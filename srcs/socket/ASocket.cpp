
#include "socket/ASocket.hpp"
#include "utils/log.hpp"

#include <unistd.h>

ASocket::ASocket(int fd, in_addr_t ip, in_port_t port)
    : _fd(fd),
    _state(SocketState::Read)
{
    _server.sin_port = port;
    _server.sin_addr.s_addr = ip;
}

int ASocket::fd() const
{
    return _fd;
}

in_addr_t   ASocket::serverIp() const
{
    return _server.sin_addr.s_addr;
}

in_port_t   ASocket::serverPort() const
{
    return _server.sin_port;
}

const sockaddr_in &ASocket::server() const
{
    return _server;
}

SocketState::_    ASocket::state() const
{
    return _state;
}

ASocket::~ASocket()
{
    close(_fd);
    logger::info << "Disconnected socket " << _fd << logger::end;
}
