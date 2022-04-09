
#include "SocketSession.hpp"

#include <sys/socket.h>

SocketSession::SocketSession(int fd)
	: ASocket(fd, TriggerType::Read)
    , _written_total(0) {}

SocketSession::SocketSession(const SocketSession &src)
	: ASocket(src.fd, src._trigger)
    , _written_total(src._written_total) {}

int     SocketSession::action(enum PostAction &post_action)
{
    if (_trigger == TriggerType::Read)
        return actionRead(post_action);
    else
        return actionWrite(post_action);
}

void    SocketSession::prepareForWrite(HTTPMessage &response)
{
    _message = response;
    _trigger = TriggerType::Write;
}


int	    SocketSession::actionRead(enum PostAction &post_action)
{
    char	temp_buffer[8192];
    ssize_t	bytes_read;

    logger::debug("Trying to read from socket", fd);
    bytes_read = recv(fd, temp_buffer, sizeof(temp_buffer) - 1, 0);
    if (bytes_read)
        logger::debug("Read from socket (bytes):", bytes_read);

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
            _trigger = TriggerType::None;
            logger::info("Got end of HTTP message from socket", fd);
        }
        else
            post_action = NoAction;
    }
    return static_cast<int>(bytes_read);
}

int	    SocketSession::actionWrite(enum PostAction &post_action)
{
    const char		*start = _message.raw_data.c_str() + _written_total;
    const size_t	left_to_write = _message.raw_data.size() - _written_total;
    ssize_t			bytes_written;

    logger::debug("Trying to write to socket", fd);
    bytes_written = send(fd, start, left_to_write, MSG_NOSIGNAL | MSG_DONTWAIT);
    logger::debug("Written to socket (bytes):", bytes_written);

    if (bytes_written > 0)
    {
        _written_total += bytes_written;
        if (_written_total == _message.raw_data.size())
        {
            logger::info("HTTP response sent. Switching to read socket", fd);
            _trigger = TriggerType::Read;
        }
        else
            logger::debug("Left to write (bytes):", left_to_write - bytes_written);
    }
    post_action = NoAction;
    return static_cast<int>(bytes_written);
}