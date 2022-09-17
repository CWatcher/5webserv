#include "HTTPResponse.hpp"
#include "utils/log.hpp"

#include <sstream>
#include <sys/socket.h>
#include <algorithm>

static const std::pair<std::string, std::string> types_init_list[] =
{
    std::make_pair("pdf", "application/pdf"),
    std::make_pair("gif", "image/gif"),
    std::make_pair("jpeg", "image/jpeg"),
    std::make_pair("png", "image/png"),
    std::make_pair("tiff", "image/tiff"),
    std::make_pair("bmp", "image/bmp"),
    std::make_pair("css", "text/css; charset=utf-8"),
    std::make_pair("csv", "text/csv; charset=utf-8"),
    std::make_pair("html", "text/html; charset=utf-8"),
    std::make_pair("txt", "text/plain; charset=utf-8"),
    std::make_pair("xml", "text/xml; charset=utf-8"),
};

const std::map<std::string, std::string>    HTTPResponse::_mime_type(types_init_list, types_init_list + sizeof(types_init_list) / sizeof(types_init_list[0]));

HTTPResponse::HTTPResponse() : HTTPMessage(), _bytes_sent(0)
{
    _header["Server"] = "webserv";
    _header["Connection"] = "keep-alive";
}

bool    HTTPResponse::send(int fd)
{
    const char		*start = _buffer.data() + _bytes_sent;
    const size_t	left_to_write = _buffer.size() - _bytes_sent;
    ssize_t			s;

    s = ::send(fd, start, std::min(BUFER_SIZE, left_to_write), MSG_NOSIGNAL | MSG_DONTWAIT);

    if (s == 0)
        throw std::exception();
    if (s == -1)
    {
        logger::error << "fd=" << fd << " send: " << logger::cerror << logger::end;
        throw std::exception();
    }

    logger::debug << "Bytes written: " << s << logger::end;
    _bytes_sent += s;
    if (_bytes_sent == _buffer.size())
        return true;
    logger::debug << "Left to write: " << _buffer.size() - _bytes_sent << logger::end;
    return false;
}

void    HTTPResponse::buildHeader(const std::string& status_line)
{
    _buffer += "HTTP/1.1 " + status_line + "\r\n";
    for (std::map<std::string, std::string>::const_iterator it = _header.begin(); it != _header.end(); ++it)
    {
        _buffer += it->first;
        _buffer += ": ";
        _buffer += it->second;
        _buffer += "\r\n";
    }
    _buffer += "\r\n";
}

void    HTTPResponse::setContentType(const std::string &file_type)
{
    std::map<std::string, std::string>::const_iterator  type = _mime_type.find(file_type);

    if (type != _mime_type.end())
        addHeader("Content-Type", type->second);
    else
        addHeader("Content-Type", "application/octet-stream");
}

void    HTTPResponse::setContentLength(size_t n)
{
    std::stringstream   ss;

    ss << n;
    addHeader("Content-Length", ss.str());
}
