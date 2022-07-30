
#include "socket/SocketSession.hpp"

#include <sys/socket.h>

SocketSession::SocketSession(int fd, in_addr_t from_listen_ip, in_port_t from_listen_port)
    : ASocket(fd, from_listen_ip, from_listen_port),
    _written_total(0) {}

void     SocketSession::action(Server *server)
{
    if (_state == SocketState::Read)
        actionRead();
    else
        actionWrite();
    if (_state == SocketState::Disconnect)
        server->deleteSession(_fd);
    else if (_state == SocketState::Process)
        server->addProcessTask(this);
}

void    SocketSession::setStateToWrite()
{
    _state = SocketState::Write;
}

void    SocketSession::actionRead()
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
        temp_buffer[bytes_read] = '\0';
        input += temp_buffer;
        if (input.hasEndOfMessage())
        {
            _state = SocketState::Process;
            logger::info << "Got end of HTTP message from socket " << _fd << logger::end;
        }
    }
}

void    SocketSession::actionWrite()
{
    const char		*start = output.raw_data.c_str() + _written_total;
    const size_t	left_to_write = output.raw_data.size() - _written_total;
    ssize_t			bytes_written;

    if (left_to_write == 0)
    {
        // где-то в обработчике это должно быть, не должно такого случаться
        const char *error = "HTTP/1.1 500\nContent-Length: 25\n\n500 Internal Server Error";

        logger::warning << "actionWrite: Empty data to write to socket " << _fd << logger::end;
        send(_fd, error, strlen(error), MSG_NOSIGNAL | MSG_DONTWAIT);
        _state = SocketState::Read;
        return;
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
            _state = SocketState::Read;
        }
        else
            logger::debug << "actionWrite: Left to write (bytes): " << left_to_write - bytes_written << logger::end;
    }
}
