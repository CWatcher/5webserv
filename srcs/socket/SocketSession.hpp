
#ifndef SOCKETSESSION_HPP
# define SOCKETSESSION_HPP

# include "HTTPMessage.hpp"
# include "socket/ASocket.hpp"

class SocketSession : public ASocket
{
public:
    explicit        SocketSession(int fd, in_addr_t from_listen_ip, in_port_t from_listen_port);

    virtual int     action();
    void            setStateToWrite();

private:
    size_t          actionRead();
    size_t          actionWrite();

public:
    HTTPMessage     input;
    HTTPMessage     output;

private:
    size_t          _written_total;
};

#endif
