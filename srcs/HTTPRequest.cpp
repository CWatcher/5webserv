#include "HTTPRequest.hpp"
#include "utils/log.hpp"
#include "utils/string.hpp"

#include <cstdlib>

void    HTTPRequest::addData(const char* data, size_t n)
{
    _raw_data.append(data, n);

    if (_header.empty())
        parseHeader();

    if (!_header.empty())
        _body_size = _raw_data.size() - _header_size;
}

bool    HTTPRequest::hasEndOfMessage()
{
    const bool          hasHeader = !_header.empty();
    const std::string   *content_length_str;
    long int            content_length;
    bool                end_found;

    if (_raw_data.compare(0, 3, "GET") || _raw_data.compare(0, 6, "DELETE"))
        end_found = hasHeader;
    else if (hasHeader && (_raw_data.compare(0, 4, "POST") || _raw_data.compare(0, 3, "PUT")))
    {
        content_length_str = getHeaderValue("Content-Length");
        if (content_length_str == NULL)
            end_found = true;
        else
        {
            content_length = std::strtol(content_length_str->c_str(), NULL, 10);
            if (errno || content_length < 0)
            {
                logger::error << __FUNCTION__ << ": " << strerror(errno) << logger::end;
                errno = 0;
                end_found = true;
            }
            else
                end_found = (_body_size >= static_cast<size_t>(content_length));
        }
    }
    else // method unknown, validator will deal with it
        end_found = true;
    return end_found;
}

const std::string   *HTTPRequest::getHeaderValue(const std::string &header_key) const
{
    std::map<std::string, std::string>::const_iterator  found;

    found = _header.find(strLowerCaseCopy(header_key));
    if (found != _header.end())
        return &found->second;
    else
        return NULL;
}

void	HTTPRequest::parseHeader()
{
    const size_t	header_end = _raw_data.find("\r\n\r\n");

    if (header_end != std::string::npos)
    {
        _header_size = header_end + 4;
        _header = getHeaderMapFromRaw();
        parseStartLine();
        logger::debug << __FUNCTION__ << ": " << "HTTP request header found" << logger::end;
    }
    else
        logger::debug << __FUNCTION__ << ": " << "HTTP request header not found yet" << logger::end;
}

void HTTPRequest::parseStartLine()
{
    //что будет в uri и method, если в starting line будет не 3 слова???

    _start_line = _raw_data.substr(0, _raw_data.find('\n'));
    size_t delimiter_index = _start_line.find(' ');
    _method = _start_line.substr(0, delimiter_index);
    if (delimiter_index != std::string::npos)
    {
        ++delimiter_index;
        _uri = _start_line.substr(delimiter_index, _start_line.find(' ', delimiter_index) - delimiter_index);
    }

    size_t  double_slash = _uri.find("//");
    while (double_slash != std::string::npos)
    {
        _uri.replace(double_slash, 2, "/");
        double_slash = _uri.find("//");
    }
}

std::map<std::string, std::string>	HTTPRequest::getHeaderMapFromRaw()
{
    std::map<std::string, std::string>	header_map;
    size_t		line_start;
    size_t		line_end;
    std::string line;

    line_end = _raw_data.find('\n', 0);
    while (true)
    {
        line_start = line_end + (_raw_data[line_end] == '\r') + 1;
        line_end = _raw_data.find('\n', line_start);
        if (_raw_data[line_end - 1] == '\r')
            --line_end;
        if (line_start == line_end)
            break ;

        line = _raw_data.substr(line_start, line_end - line_start);
        header_map.insert(getHeaderPairFromLine(line));
    }
    return header_map;
}

std::pair<std::string, std::string>	HTTPRequest::getHeaderPairFromLine(const std::string &line)
{
    const size_t	key_end = line.find(':');
    std::string		key;
    std::string		value;

    if (key_end == std::string::npos)
    {
        key = line;
        value = "";
    }
    else
    {
        key = line.substr(0, key_end);
        value = line.substr(key_end + 1, std::string::npos);
    }
    strTrim(strLowerCase(key));
    strTrim(value);

    return std::make_pair(key, value);
}

const   std::string HTTPRequest::getHeaderHostName() const
{
    const std::string *host = getHeaderValue("Host");

    if (host == NULL)
        return std::string();

    return host->substr(0, host->find(':'));
}
