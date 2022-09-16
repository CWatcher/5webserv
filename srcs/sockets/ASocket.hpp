
#ifndef ASOCKET_HPP
# define ASOCKET_HPP

# include <netinet/in.h>

# define BUFER_SIZE 8192

namespace SocketState {
enum _ {
    Read,
    Write,
    Handle,
    Disconnect
};
}

class ASocket
{
public:
    ASocket(int fd, in_addr_t ip, in_port_t port);
    ASocket(const ASocket &src);
    virtual	~ASocket();

    virtual int     action(in_addr &remote_addr) = 0;
    int             fd() const;
    in_addr_t       ip() const;
    in_port_t       port() const;
    SocketState::_  state() const;

protected:
    const int       _fd;
    const in_addr_t _ip;
    const in_port_t _port;
    SocketState::_  _state;
};

#endif
