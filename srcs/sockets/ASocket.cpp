
#include "sockets/ASocket.hpp"
#include "utils/log.hpp"

#include <unistd.h>

ASocket::ASocket(int fd, in_addr_t ip, in_port_t port)
    : _fd(fd),
    _ip(ip),
    _port(port),
    _state(SocketState::Read) {}

int ASocket::fd() const
{
    return _fd;
}

in_addr_t   ASocket::ip() const
{
    return _ip;
}

in_port_t   ASocket::port() const
{
    return _port;
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
