#include "HTTPResponse.hpp"

#include <sstream>

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
    std::make_pair("txt", "text/html; charset=utf-8"),
    std::make_pair("xml", "text/xml; charset=utf-8"),
};

const std::map<std::string, std::string>    HTTPResponse::_mime_type(types_init_list, types_init_list + sizeof(types_init_list) / sizeof(types_init_list[0]));

void    HTTPResponse::buildResponse(const char* body, size_t n, const std::string& status_line)
{
    setContentLength(n);
    buildHeader(status_line);
    _raw_data.append(body, n);
}

void    HTTPResponse::buildHeader(const std::string& status_line)
{
    _raw_data += "HTTP/1.1 " + status_line + "\r\n";
    for (std::map<std::string, std::string>::const_iterator it = _header.begin(); it != _header.end(); ++it)
    {
        _raw_data += it->first;
        _raw_data += ": ";
        _raw_data += it->second;
        _raw_data += "\r\n";
    }
    _raw_data += "\r\n";
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
