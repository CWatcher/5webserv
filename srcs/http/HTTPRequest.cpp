#include "HTTPRequest.hpp"
#include "utils/log.hpp"
#include "utils/string.hpp"

#include <sstream>

const std::string    HTTPRequest::terminator("\r\n");

void    HTTPRequest::addData(const char* data, size_t n)
{
    _raw_data.append(data, n);

    if (_header.empty())
        fillHeaderMap();

    if (!_header.empty())
        _body_size = _raw_data.size() - _header_size;
}

bool    HTTPRequest::isRequestReceived()
{
    if (_header.empty())
        return false;

    std::string content_length = getHeaderValue("Content-Length");
    std::string transfer_encoding = getHeaderValue("Transfer-Encoding");
    if (transfer_encoding.find("chunked") != std::string::npos)
        return unchunk();
        /**
        обновить body_size - размер собранного запроса
        убрать chuncked из заголовка добавить Content-Length = body_size
        **/
    else if (!content_length.empty())
    {
        std::stringstream converter(content_length);
        size_t  length;

        converter >> length;
        if (!converter.eof())
        {
            logger::error << "HTTPRequest: Content-Length bad value" << logger::end;
            throw std::exception();
        }
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
    const size_t    header_end = _raw_data.find("\r\n\r\n");

    if (header_end != std::string::npos)
    {
        _header_size = header_end + 4;
        _chunk_pos = _header_size;
        parseStartLine();
        parseHeader(header_end);
        logger::debug << "HTTPRequest:" << "HTTP request header found" << logger::end;
    }
    else
        logger::debug << "HTTPRequest:" << "HTTP request header not found yet" << logger::end;
}

void    HTTPRequest::parseStartLine()
{
    std::string _start_line = _raw_data.substr(0, _raw_data.find("\r\n"));
    size_t      delimiter_idx = _start_line.find(' ');

    _method = _start_line.substr(0, delimiter_idx);

    ++delimiter_idx;
    _uri = _start_line.substr(delimiter_idx, _start_line.rfind(' ') - delimiter_idx);
    strRemoveDoubled(_uri, '/');

    _http = _start_line.substr(_start_line.rfind(' ') + 1);
}

void    HTTPRequest::parseHeader(size_t header_end)
{
    size_t  line_start = _raw_data.find('\n') + 1;
    size_t  line_end = _raw_data.find('\n', line_start);

    while (line_start < header_end)
    {
        std::string line = _raw_data.substr(line_start, line_end - line_start);

        parseHeaderLine(line);
        line_start = ++line_end;
        line_end = _raw_data.find('\n', line_end);
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

bool HTTPRequest::unchunk()
{

    while (_chunk_pos < _raw_data.size())
    {
        if (_chunk_size == std::numeric_limits<std::size_t>::max())
        {
            size_t end  = _raw_data.find(terminator, _chunk_pos);
            if (end == std::string::npos)
                return false;
            std::stringstream converter(_raw_data.substr(_chunk_pos, end - _chunk_pos));
            converter >> std::hex >> _chunk_size;
            if (converter.fail())
            //TODO check where it is caught
                throw (std::runtime_error(std::string("bad chunk size")));
            _raw_data.erase(_chunk_pos, end + terminator.size() - _chunk_pos);
        }
        else if(_chunk_size == 0)
        {
            size_t end  = _raw_data.find(terminator, _chunk_pos);
            if (end == std::string::npos)
                return false;
            _raw_data.erase(_chunk_pos);
            return true;
        }
        else
        {
            if (_raw_data.size() < _chunk_pos + _chunk_size + terminator.size())
                return false;
            _chunk_pos += _chunk_size;
            if (_raw_data.substr(_chunk_pos, terminator.size()) != terminator)
                //TODO check where it is caught
                throw (std::runtime_error(std::string("bad chunk terminator")));
            _raw_data.erase(_chunk_pos, terminator.size());
            _chunk_size = std::numeric_limits<std::size_t>::max();
        }
    }
    return false;
}
