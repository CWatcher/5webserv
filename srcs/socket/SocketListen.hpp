
#ifndef SOCKETLISTEN_HPP
# define SOCKETLISTEN_HPP

# include "socket/ASocket.hpp"

const int ConnectionsLimit = 1024;

class SocketListen : public ASocket
{
public:
    SocketListen(in_addr_t ip, in_port_t port, int connections_limit = ConnectionsLimit);

    virtual void    action(Server *server);
};

#endif
