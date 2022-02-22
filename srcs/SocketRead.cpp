
#include "SocketRead.hpp"

#include <sys/socket.h>

SocketRead::SocketRead(int fd)
	: ASocket(fd, TriggerType::Read) {}

SocketRead::SocketRead(const ASocket &src)
	: ASocket(src.fd, TriggerType::Read) {}

int	SocketRead::action(enum PostAction &post_action)
{
	char	temp_buffer[8192];
	ssize_t	bytes_read;

	log::debug("Trying to read from socket", fd);
	bytes_read = recv(fd, temp_buffer, sizeof(temp_buffer) - 1, 0);
	if (bytes_read)
		log::debug("Read from socket (bytes):", bytes_read);

	if (bytes_read == -1)
		post_action = NoAction;
	else if (bytes_read == 0)
		post_action = Disconnect;
	else
	{
		temp_buffer[bytes_read] = '\0';
		_message += temp_buffer;
		if (_message.hasEndOfMessage())
		{
			post_action = Process;
			log::info("Got end of HTTP message from socket", fd);
		}
		else
			post_action = NoAction;
	}
	return static_cast<int>(bytes_read);
}
