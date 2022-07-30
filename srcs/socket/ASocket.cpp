
#include "socket/ASocket.hpp"
#include "utils/log.hpp"

#include <unistd.h>

ASocket::ASocket(int fd, in_addr_t ip, in_port_t port)
    : _fd(fd),
    _ip(ip),
    _port(port),
    _state(SocketState::Read) {}

ASocket::ASocket(const ASocket &src)
    : _fd(src._fd),
    _ip(src._ip),
    _port(src._port) {}

int ASocket::fd() const
{
    return _fd;
}

int ASocket::ip() const
{
    return _ip;
}

int ASocket::port() const
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
