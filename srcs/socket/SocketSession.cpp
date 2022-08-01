
#include "socket/SocketSession.hpp"

#include <sys/socket.h>

SocketSession::SocketSession(int fd, in_addr_t from_listen_ip, in_port_t from_listen_port)
    : ASocket(fd, from_listen_ip, from_listen_port),
    _written_total(0) {}

int     SocketSession::action()
{
    size_t  ret;

    if (_state == SocketState::Read)
        try
        {
            ret = actionRead();
        }
        catch (const std::bad_alloc &e)
        {
            //output status 500
            logger::error << "SocketSession: actionRead: " << e.what() << logger::end;
        }
    else
        ret = actionWrite();
    if (ret == -1ul)
        logger::error << "SocketSession: recv/send: " << logger::cerror << logger::end;
    return -1;
}

void    SocketSession::setStateToWrite()
{
    _written_total = 0;
    _state = SocketState::Write;
}

size_t  SocketSession::actionRead()
{
    char	temp_buffer[8192];
    ssize_t	bytes_read;

    logger::debug << "Trying to read from socket " << _fd << logger::end;
    bytes_read = recv(_fd, temp_buffer, sizeof(temp_buffer) - 1, 0);
    if (bytes_read)
        logger::debug << "Read from socket (bytes): " << bytes_read << logger::end;

    if (bytes_read == -1)
    {
        //очистить буффер и отключить клиента или ничего не делать?
    }
    else if (bytes_read == 0)
        _state = SocketState::Disconnect;
    else
    {
        _request.append(temp_buffer, bytes_read);
        if (_request.hasEndOfMessage())
        {
            _state = SocketState::Process;
            logger::info << "Got end of HTTP message from socket " << _fd << logger::end;
        }
    }
    return bytes_read;
}

size_t  SocketSession::actionWrite()
{
    const char		*start = output.raw_data.c_str() + _written_total;
    const size_t	left_to_write = output.raw_data.size() - _written_total;
    ssize_t			bytes_written;

    if (left_to_write == 0)
    {
        // где-то в обработчике это должно быть, не должно такого случаться
        const char *error = "HTTP/1.1 500\nContent-Length: 25\n\n500 Internal Server Error";

        logger::warning << "actionWrite: Empty data to write to socket " << _fd << logger::end;
        bytes_written = send(_fd, error, strlen(error), MSG_NOSIGNAL | MSG_DONTWAIT);
        _request = HTTPRequest();
        _state = SocketState::Read;
        return bytes_written;
    }

    logger::debug << "actionWrite: Trying to write to socket " << _fd << logger::end;
    bytes_written = send(_fd, start, left_to_write, MSG_NOSIGNAL | MSG_DONTWAIT);
    logger::debug << "actionWrite: Written to socket (bytes): " << bytes_written << logger::end;

    if (bytes_written > 0)
    {
        _written_total += bytes_written;
        if (_written_total == output.raw_data.size())
        {
            logger::info << "actionWrite: HTTP response sent. Switching to read socket " << _fd << logger::end;
            _request = HTTPRequest();
            _state = SocketState::Read;
        }
        else
            logger::debug << "actionWrite: Left to write (bytes): " << left_to_write - bytes_written << logger::end;
    }
    return bytes_written;
}
