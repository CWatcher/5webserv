#include "HTTPRequest.hpp"
#include "utils/log.hpp"
#include "utils/string.hpp"

#include <cstdlib>

void    HTTPRequest::addData(const char* data, size_t n)
{
    _raw_data.append(data, n);

    if (_header.empty())
        makeHeaderMap();

    if (!_header.empty()) ///
        _body_size = _raw_data.size() - _header_size;
}

bool    HTTPRequest::isRequestReceived()
{
    const bool          header_received = !_header.empty();
    long int            content_length;
    bool                end_found;

    if (_raw_data.compare(0, 3, "GET") || _raw_data.compare(0, 6, "DELETE"))
        end_found = header_received;
    else if (header_received && (_raw_data.compare(0, 4, "POST") || _raw_data.compare(0, 3, "PUT")))
    {
        const std::string   content_length_str = getHeaderValue("Content-Length");
        if (content_length_str.empty())
            end_found = true;
        else
        {
            content_length = std::strtol(content_length_str.c_str(), NULL, 10);
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

std::string HTTPRequest::getHeaderValue(const std::string &header_key) const
{
    std::map<std::string, std::string>::const_iterator  found;

    found = _header.find(strLowerCaseCopy(header_key));
    if (found != _header.end())
        return found->second;
    else
        return "";
}

void	HTTPRequest::makeHeaderMap()
{
    const size_t    header_end = _raw_data.find("\r\n\r\n");

    if (header_end != std::string::npos)
    {
        _header_size = header_end + 4;
        parseStartLine();
        parseHeader(header_end);
        logger::debug << "HTTPRequest:" << "HTTP request header found" << logger::end;
    }
    else
        logger::debug << "HTTPRequest:" << "HTTP request header not found yet" << logger::end;
}

void HTTPRequest::parseStartLine()
{
    std::string _start_line = _raw_data.substr(0, _raw_data.find('\r'));
    size_t      delimiter_index = _start_line.find(' ');

    _method = _start_line.substr(0, delimiter_index);
    ++delimiter_index;
    _uri = _start_line.substr(delimiter_index, _start_line.rfind(' ') - delimiter_index);
    strRemoveDoubled(_uri, '/');
    _http = _start_line.substr(_start_line.rfind(' ') + 1);
}

void    HTTPRequest::parseHeader(size_t header_end)
{
    size_t  line_start = 0;
    size_t  line_end = _raw_data.find('\n');

    while (line_end < header_end)
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
    std::string     key = line.substr(0, key_end);;
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
