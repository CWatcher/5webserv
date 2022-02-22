
#ifndef SOCKETLISTEN_HPP
# define SOCKETLISTEN_HPP

# include "ASocket.hpp"

# include <netinet/in.h>

class SocketListen : public ASocket
{
public:
	SocketListen(in_port_t port, int connections_limit);

	virtual int	action(enum PostAction &post_action);
};

#endif
