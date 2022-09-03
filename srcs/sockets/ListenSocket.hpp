#ifndef LISTENSOCKET_HPP
# define LISTENSOCKET_HPP

# include "sockets/ASocket.hpp"

class ListenSocket : public ASocket
{
public:
    ListenSocket(in_addr_t ip, in_port_t port, int connections_limit = ConnectionsLimit);

    virtual int action(in_addr &remote_addr);

    static int ConnectionsLimit;
};

#endif
