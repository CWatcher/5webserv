
#ifndef SOCKETREAD_HPP
# define SOCKETREAD_HPP

# include "ASocket.hpp"
# include "HTTPMessage.hpp"

class SocketRead : public ASocket
{
public:
	explicit SocketRead(int fd);
	explicit SocketRead(const ASocket &src);

	virtual int	action(enum PostAction &post_action);

private:
	HTTPMessage	_message;
};

#endif
