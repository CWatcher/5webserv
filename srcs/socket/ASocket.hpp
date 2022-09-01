
#ifndef ASOCKET_HPP
# define ASOCKET_HPP

# include <netinet/in.h>

namespace SocketState { enum _ {
    Read,
    Write,
    Process,
    Disconnect
}; }

class ASocket
{
public:
    ASocket(int fd, in_addr_t ip, in_port_t port);
    virtual	~ASocket();

    virtual int     action(sockaddr_in & remote_addr) = 0;
    int             fd() const;
    in_addr_t       serverIp() const;
    in_port_t       serverPort() const;
    SocketState::_  state() const;

private:
    ASocket();
    ASocket	&operator=(const ASocket &rhs);

protected:
    const int       _fd;
    sockaddr_in     _server;
    SocketState::_  _state;
};

#endif
