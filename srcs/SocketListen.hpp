
#ifndef SOCKETLISTEN_HPP
# define SOCKETLISTEN_HPP

# include "ASocket.hpp"

# include <netinet/in.h>
# include <arpa/inet.h>

const int ConnectionsLimit = 1024;

class SocketListen : public ASocket
{
public:
    SocketListen(in_addr_t ip, in_port_t port, int connections_limit = ConnectionsLimit);

	virtual int	action(enum PostAction &post_action);

    in_addr_t ip;
    in_port_t port;
};

#endif
