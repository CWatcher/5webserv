#include "HTTPRequest.hpp"
#include "utils/log.hpp"
#include "utils/string.hpp"

#include <sstream>
#include <sys/socket.h>

const std::string    HTTPRequest::terminator("\r\n");

bool    HTTPRequest::read(int fd)
{
    char    data_buffer[BUFFER_SIZE];
    ssize_t r;

    r = ::recv(fd, data_buffer, BUFFER_SIZE, MSG_NOSIGNAL | MSG_DONTWAIT);

    if (r == 0)
        throw std::exception();
    if (r == -1)
        throw std::runtime_error(::strerror(errno));

    logger::debug << "HTTPRequest: read " << r << " bytes from " << fd << logger::end;
    _buffer.append(data_buffer, r);
    if (_header.empty())
        fillHeaderMap();
    if (!_header.empty())
        _body_size = _buffer.size() - _header_size;

    return isRequestReceived();
}

bool    HTTPRequest::isRequestReceived()
{
    if (_header.empty())
        return false;

    std::string content_length = getHeaderValue("Content-Length");
    std::string transfer_encoding = getHeaderValue("Transfer-Encoding");
    std::string chanked_encoding = "chunked";
    if (transfer_encoding.find(chanked_encoding) != std::string::npos)
    {
        if (!dechunk())
            return false;
        _body_size = _buffer.size() - _header_size;
        transfer_encoding.erase(transfer_encoding.find(chanked_encoding), chanked_encoding.size());
        strRemoveDoubled(transfer_encoding, ' ');
        if (transfer_encoding.size() == 0)
            _header.erase("Transfer-Encoding");
        else
            _header["Transfer-Encoding"] = transfer_encoding;
        _header["Content-Length"] = _body_size;
        return true;
    }
    else if (!content_length.empty())
    {
        std::stringstream converter(content_length);
        size_t  length;

        converter >> length;
        if (!converter.eof())
            throw std::runtime_error("HTTPRequest: Content-Length bad value");
        if (length > _body_size)
            return false;
        return true;
    }

    return true;
}

std::string HTTPRequest::getHeaderValue(const std::string &header_key) const
{
    std::map<std::string, std::string>::const_iterator  found;

    found = _header.find(strLowerCaseCopy(header_key));
    if (found != _header.end())
        return found->second;
    else
        return "";
}

void    HTTPRequest::fillHeaderMap()
{
    const size_t    header_end = _buffer.find("\r\n\r\n");

    if (header_end != std::string::npos)
    {
        _header_size = header_end + 4;
        _chunk_pos = _header_size;
        parseStartLine();
        parseHeader(header_end);
        logger::debug << "HTTPRequest: request header found" << logger::end;
    }
    else
        logger::debug << "HTTPRequest: request header not found yet" << logger::end;
}

void    HTTPRequest::parseStartLine()
{
    std::string _start_line = _buffer.substr(0, _buffer.find("\r\n"));
    size_t      delimiter_idx = _start_line.find(' ');

    _method = _start_line.substr(0, delimiter_idx);

    ++delimiter_idx;
    _uri = _start_line.substr(delimiter_idx, _start_line.rfind(' ') - delimiter_idx);
    strRemoveDoubled(_uri, '/');

    _http = _start_line.substr(_start_line.rfind(' ') + 1);
}

void    HTTPRequest::parseHeader(size_t header_end)
{
    size_t  line_start = _buffer.find('\n') + 1;
    size_t  line_end = _buffer.find('\n', line_start);

    while (line_start < header_end)
    {
        std::string line = _buffer.substr(line_start, line_end - line_start);

        parseHeaderLine(line);
        line_start = ++line_end;
        line_end = _buffer.find('\n', line_end);
    }
}

void    HTTPRequest::parseHeaderLine(const std::string &line)
{
    const size_t    key_end = line.find(':');
    std::string     key = line.substr(0, key_end);
    std::string     value = line.substr(key_end + 1);

    strLowerCase(key);
    strTrim(key);
    strTrim(value);
    _header[key] = value;
}

std::string HTTPRequest::getHeaderHostName() const
{
    const std::string host = getHeaderValue("Host");

    if (host.empty())
        return "";

    return host.substr(0, host.find(':'));
}

std::string HTTPRequest::getHeaderParameter(const std::string& key, const std::string& param) const
{
    std::map<std::string, std::string>::const_iterator  found;

    found = _header.find(strLowerCaseCopy(key));
    if (found == _header.end())
        return "";

    const std::vector<std::string>                      parameters = strSplit(found->second, ';');
    for (std::vector<std::string>::const_iterator it = parameters.begin(); it != parameters.end(); ++it)
        if (it->find(param) != std::string::npos)
            try
            {
                return it->substr(it->find('=') + 1);
            }
            catch(const std::out_of_range&)
            {
                return "";
            }

    return "";
}

bool    HTTPRequest::dechunk()
{
    while (_chunk_size != 0)
        if (_chunk_size == std::numeric_limits<std::size_t>::max())
        {
            size_t end  = _buffer.find(terminator, _chunk_pos);
            if (end == std::string::npos)
                return false;
            std::stringstream converter(_buffer.substr(_chunk_pos, end - _chunk_pos));
            converter >> std::hex >> _chunk_size;
            if (!converter || !converter.eof())
                throw std::runtime_error("bad chunk size");
            _buffer.erase(_chunk_pos, end + terminator.size() - _chunk_pos);
        }
        else
        {
            if (_buffer.size() < _chunk_pos + _chunk_size + terminator.size())
                return false;
            _chunk_pos += _chunk_size;
            if (_buffer.substr(_chunk_pos, terminator.size()) != terminator)
                throw std::runtime_error("bad chunk terminator");
            _buffer.erase(_chunk_pos, terminator.size());
            _chunk_size = std::numeric_limits<std::size_t>::max();
        }
    if (_buffer.find(terminator, _chunk_pos) == std::string::npos)
        return false;
    _buffer.erase(_chunk_pos);
    return true;
}
