
#include "SocketSession.hpp"

#include <csignal>
#include <sys/socket.h>

SocketSession::SocketSession(int fd, in_addr_t from_listen_ip, in_port_t from_listen_port)
	: ASocket(fd, TriggerType::Read)
    , from_listen_address(std::make_pair<in_addr_t, in_port_t>(from_listen_ip, from_listen_port))
    , _written_total(0) {}

int     SocketSession::action(enum PostAction &post_action)
{
    if (_trigger == TriggerType::Read)
        return actionRead(post_action);
    else
        return actionWrite(post_action);
}

void    SocketSession::prepareForRead()
{
    input = HTTPMessage();
    _trigger = TriggerType::Read;
}

void    SocketSession::prepareForProcess()
{
    _trigger = TriggerType::None;
}

void    SocketSession::prepareForWrite()
{
    _written_total = 0;
    _trigger = TriggerType::Write;
}


int	    SocketSession::actionRead(enum PostAction &post_action)
{
    char	temp_buffer[8192];
    ssize_t	bytes_read;

    logger::debug << "Trying to read from socket " << fd << logger::end;
    bytes_read = recv(fd, temp_buffer, sizeof(temp_buffer) - 1, 0);
    if (bytes_read)
        logger::debug << "Read from socket (bytes): " << bytes_read << logger::end;

    if (bytes_read == -1)
        post_action = NoAction;
    else if (bytes_read == 0)
        post_action = Disconnect;
    else
    {
        temp_buffer[bytes_read] = '\0';
        input += temp_buffer;
        if (input.hasEndOfMessage())
        {
            post_action = Process;
            prepareForProcess();
            logger::info << "Got end of HTTP message from socket " << fd << logger::end;
        }
        else
            post_action = NoAction;
    }
    return static_cast<int>(bytes_read);
}

int	    SocketSession::actionWrite(enum PostAction &post_action)
{
    const char		*start = output.raw_data.c_str() + _written_total;
    const size_t	left_to_write = output.raw_data.size() - _written_total;
    ssize_t			bytes_written;

    if (left_to_write == 0)
    {
        const char *error = "HTTP/1.1 500\nContent-Length: 25\n\n500 Internal Server Error";

        logger::warning << "actionWrite: Empty data to write to socket " << fd << logger::end;
        send(fd, error, strlen(error), MSG_NOSIGNAL | MSG_DONTWAIT);
        prepareForRead();
        post_action = NoAction;
        return 0;
    }

    logger::debug << "actionWrite: Trying to write to socket " << fd << logger::end;
    bytes_written = send(fd, start, left_to_write, MSG_NOSIGNAL | MSG_DONTWAIT);
    logger::debug << "actionWrite: Written to socket (bytes): " << bytes_written << logger::end;

    if (bytes_written > 0)
    {
        _written_total += bytes_written;
        if (_written_total == output.raw_data.size())
        {
            logger::info << "actionWrite: HTTP response sent. Switching to read socket " << fd << logger::end;
            prepareForRead();
        }
        else
            logger::debug << "actionWrite: Left to write (bytes): " << left_to_write - bytes_written << logger::end;
    }
    post_action = NoAction;
    return static_cast<int>(bytes_written);
}
