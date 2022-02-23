
#ifndef SOCKETSESSION_HPP
# define SOCKETSESSION_HPP

# include "ASocket.hpp"
# include "HTTPMessage.hpp"

class SocketSession : public ASocket
{
public:
	explicit SocketSession(int fd);
	SocketSession(const SocketSession &src);

	virtual int action(enum PostAction &post_action);
    void        prepareForWrite(HTTPMessage &response);

private:
	int         actionRead(enum PostAction &post_action);
	int         actionWrite(enum PostAction &post_action);


private:
	HTTPMessage	_message;
    size_t		_written_total;
};

#endif
