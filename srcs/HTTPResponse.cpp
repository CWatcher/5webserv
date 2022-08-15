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
    std::make_pair("css", "text/css"),
    std::make_pair("csv", "text/csv"),
    std::make_pair("html", "text/html"),
    std::make_pair("plain", "text/plain"),
    std::make_pair("xml", "text/xml"),
};

const std::map<std::string, std::string>    HTTPResponse::_mime_type(types_init_list, types_init_list + sizeof(types_init_list) / sizeof(types_init_list[0]));

const std::pair<HTTPStatus, std::string>    http_status_init_list[] =
{
    std::make_pair(CONTINUE, "Continue"),
    std::make_pair(SWITCHING_PROTOCOL, "Switching Protocol"),
    std::make_pair(PROCESSING, "Processing"),
    std::make_pair(EARLY_HINTS,	"Early Hints"),
    std::make_pair(OK, "OK"),
    std::make_pair(CREATED,	"Created"),
    std::make_pair(ACCEPTED, "Accepted"),
    std::make_pair(NON_AUTHORITATIVE_INFORMATION, "Non-Authoritative Information"),
    std::make_pair(NO_CONTENT, "No Content"),
    std::make_pair(RESET_CONTENT, "Reset Content"),
    std::make_pair(PARTIAL_CONTENT, "Partial Content"),
    std::make_pair(MULTIPLE_CHOICE, "Multiple Choice"),
    std::make_pair(MOVED_PERMANENTLY, "Moved Permanently"),
    std::make_pair(FOUND, "Found"),
    std::make_pair(SEE_OTHER, "See Other"),
    std::make_pair(NOT_MODIFIED, "Not Modified"),
    std::make_pair(USE_PROXY, "Use Proxy"),
    std::make_pair(SWITCH_PROXY, "Switch Proxy"),
    std::make_pair(TEMPORARY_REDIRECT, "Temporary Redirect"),
    std::make_pair(PERMANENT_REDIRECT, "Permanent Redirect"),
    std::make_pair(BAD_REQUEST, "Bad Request"),
    std::make_pair(UNAUTHORIZED, "Unauthorized"),
    std::make_pair(PAYMENT_REQUIRED, "Payment Required"),
    std::make_pair(FORBIDDEN, "Forbidden"),
    std::make_pair(NOT_FOUND, "Not Found"),
    std::make_pair(METHOD_NOT_ALLOWED, "Method Not Allowed"),
    std::make_pair(NOT_ACCEPTABLE, "Not Acceptable"),
    std::make_pair(PROXY_AUTHENTICATION_REQUIRED, "Proxy Authentication Required"),
    std::make_pair(REQUEST_TIMEOUT, "Request Timeout"),
    std::make_pair(CONFLICT, "Conflict"),
    std::make_pair(GONE, "Gone"),
    std::make_pair(LENGTH_REQUIRED, "Length Required"),
    std::make_pair(PRECONDITION_FAILED, "Precondition Failed"),
    std::make_pair(REQUEST_ENTITY_TOO_LARGE, "Request Entity Too Large"),
    std::make_pair(REQUEST_URI_TOO_LONG, "Request-URI Too Long"),
    std::make_pair(UNSUPPORTED_MEDIA_TYPE, "Unsupported Media Type"),
    std::make_pair(REQUESTED_RANGE_NOT_SATISFIABLE, "Requested Range Not Satisfiable"),
    std::make_pair(EXPECTATION_FAILED, "Expectation Failed"),
    std::make_pair(INTERNAL_SERVER_ERROR, "Internal Server Error"),
    std::make_pair(NOT_IMPLEMENTED, "Not Implemented"),
    std::make_pair(BAD_GATEWAY, "Bad Gateway"),
    std::make_pair(SERVICE_UNAVAILABLE, "Service Unavailable"),
    std::make_pair(GATEWAY_TIMEOUT, "Gateway Timeout"),
    std::make_pair(HTTP_VERSION_NOT_SUPPORTED, "HTTP Version Not Supported")
};

const std::map<HTTPStatus, std::string>	http_status_(http_status_init_list, http_status_init_list + sizeof(http_status_init_list) / sizeof(http_status_init_list[0]));

void	HTTPResponse::buildResponse(const char* body, size_t n, HTTPStatus status)
{
    std::stringstream   ss;

    ss << status;
    _raw_data += "HTTP/1.1 " + ss.str() + " " + http_status_.find(status)->second + "\n";
    for (std::map<std::string, std::string>::const_iterator it = _header.begin(); it != _header.end(); ++it)
    {
        _raw_data += it->first;
        _raw_data += " : ";
        _raw_data += it->second;
        _raw_data.push_back('\n');
    }
    _raw_data.push_back('\n');
    _raw_data.append(body, n);
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
    (void)n;
}
