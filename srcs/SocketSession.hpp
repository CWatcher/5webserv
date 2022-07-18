
#ifndef SOCKETSESSION_HPP
# define SOCKETSESSION_HPP

# include "ASocket.hpp"
# include "HTTPMessage.hpp"

# include <netinet/in.h>

class SocketSession : public ASocket
{
public:
	explicit SocketSession(int fd, in_addr_t from_listen_ip, in_port_t from_listen_port);

	virtual int action(enum PostAction &post_action);
    void        prepareForRead();
    void        prepareForProcess();
    void        prepareForWrite();

private:
	int         actionRead(enum PostAction &post_action);
	int         actionWrite(enum PostAction &post_action);

public:
    std::pair<in_addr_t, in_port_t> from_listen_address;

    HTTPMessage	input;
    HTTPMessage	output;

private:
    size_t		_written_total;
};

#endif
