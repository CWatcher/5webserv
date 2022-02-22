
#include "SocketWrite.hpp"

#include <sys/socket.h>

const char		*DefaultResponse =	"HTTP/1.1 200 OK\n"
										"Date: Wed, 18 Feb 2021 11:20:59 GMT\n"
										"Server: Apache\n"
										"X-Powered-By: webserv\n"
										"Last-Modified: Wed, 11 Feb 2009 11:20:59 GMT\n"
										"Content-Type: text/html; charset=utf-8\n"
										"Content-Length: 14\n"
										"Connection: close\n"
										"\n"
										"<h1>HELLO WORLD</h1>\n";

SocketWrite::SocketWrite(int fd)
	: ASocket(fd, TriggerType::Write)
	, _written_total(0) {}

SocketWrite::SocketWrite(const ASocket &src)
	: ASocket(src.fd, TriggerType::Write)
	, _written_total(0)
{
	_message.raw_data = DefaultResponse; // TODO: DELETE ME LATER!!
}

int	SocketWrite::action(enum PostAction &post_action)
{
	const char		*start = _message.raw_data.c_str() + _written_total;
	const size_t	left_to_write = _message.raw_data.size() - _written_total;
	ssize_t			bytes_written;

	log::debug("Trying to write to socket", fd);
	bytes_written = send(fd, start, left_to_write, MSG_NOSIGNAL | MSG_DONTWAIT);
	log::debug("Written to socket (bytes):", bytes_written);

	if (bytes_written <= 0)
		post_action = NoAction;
	else
	{
		_written_total += bytes_written;
		if (_written_total == _message.raw_data.size())
		{
			log::info("HTTP response sent. Switching to read socket", fd);
			post_action = Read;
		}
		else
		{
			log::debug("Left to write (bytes):", left_to_write - bytes_written);
			post_action = NoAction;
		}
	}
	return static_cast<int>(bytes_written);
}
