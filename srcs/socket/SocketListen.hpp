#pragma once

# include "socket/ASocket.hpp"

class SocketListen : public ASocket
{
public:
    SocketListen(in_addr_t ip, in_port_t port, int connections_limit = ConnectionsLimit);

    virtual int action();

    static int ConnectionsLimit;
};
