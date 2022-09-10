
#include "sockets/SessionSocket.hpp"
#include "utils/log.hpp"

#include <sys/socket.h>

SessionSocket::SessionSocket(int fd, in_addr_t from_listen_ip, in_port_t from_listen_port, const in_addr &remote_addr) :
    ASocket(fd, from_listen_ip, from_listen_port),
    _remote_addr(remote_addr),
    _written_total(0)
{}

int     SessionSocket::action(in_addr &)
{
    if (_state == SocketState::Read)
        readRequest();
    else
        sendResponse();
    return -1;
}

void    SessionSocket::setStateToWrite()
{
    _written_total = 0;
    _state = SocketState::Write;
}

size_t  SessionSocket::readRequest()
{
    char	temp_buffer[8192];
    ssize_t	bytes_read;

    logger::debug << "Trying to read from socket " << _fd << logger::end;
    bytes_read = recv(_fd, temp_buffer, sizeof(temp_buffer) - 1, MSG_NOSIGNAL | MSG_DONTWAIT);
    if (bytes_read)
        logger::debug << "Read from socket (bytes): " << bytes_read << logger::end;

    if (bytes_read == -1)
        logger::error << "SessionSocket: readRequest: recv: " << logger::cerror << logger::end;
    if (bytes_read <= 0)
        _state = SocketState::Disconnect;
    else
    {
        try
        {
            _request.addData(temp_buffer, bytes_read);
            if (_request.isRequestReceived())
            {
                _state = SocketState::Handle;
                logger::info << "Got end of HTTP message from socket " << _fd << logger::end;
            }
        }
        catch (const std::bad_alloc &e)
        {
            logger::error << "HTTPRequest: addData: " << e.what() << logger::end;
            _state = SocketState::Disconnect;
        }
    }
    return bytes_read;
}

size_t  SessionSocket::sendResponse()
{
    const char		*start = _response.raw_data().data() + _written_total;
    const size_t	left_to_write = _response.raw_data().size() - _written_total;
    ssize_t			bytes_written;

    if (left_to_write == 0)
    {
        // где-то в обработчике это должно быть, не должно такого случаться
        const char *error = "HTTP/1.1 500\nContent-Length: 25\n\n500 Internal Server Error";

        logger::warning << "sendResponse: Empty data to write to socket " << _fd << logger::end;
        bytes_written = send(_fd, error, strlen(error), MSG_NOSIGNAL | MSG_DONTWAIT);
        _request = HTTPRequest();
        _state = SocketState::Read;
        return bytes_written;
    }

    logger::debug << "sendResponse: Trying to write to socket " << _fd << logger::end;
    bytes_written = send(_fd, start, left_to_write, MSG_NOSIGNAL | MSG_DONTWAIT);
    logger::debug << "sendResponse: Written to socket (bytes): " << bytes_written << logger::end;
    if (bytes_written == -1)
        logger::error << "SessionSocket: sendResponse: send: " << logger::cerror << logger::end;
    if (bytes_written <= 0)
         _state = SocketState::Disconnect;
    else
    {
        _written_total += bytes_written;
        if (_written_total == _response.raw_data().size())
        {
            logger::info << "sendResponse: HTTP response sent. Switching to read socket " << _fd << logger::end;

            if (_request.getHeaderValue("Connection") == "close")
                _state = SocketState::Disconnect;
            else
                _state = SocketState::Read;
            _request = HTTPRequest();
            _response = HTTPResponse();
        }
        else
            logger::debug << "sendResponse: Left to write (bytes): " << left_to_write - bytes_written << logger::end;
    }
    return bytes_written;
}
