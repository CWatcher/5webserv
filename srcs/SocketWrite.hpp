
#ifndef SOCKETWRITE_HPP
# define SOCKETWRITE_HPP

# include "ASocket.hpp"
# include "HTTPMessage.hpp"

class SocketWrite : public ASocket
{
public:
	explicit	SocketWrite(int fd);
	explicit	SocketWrite(const ASocket &src);
	virtual int	action(enum PostAction &post_action);

private:
	HTTPMessage	_message;
	size_t		_written_total;
};

#endif
