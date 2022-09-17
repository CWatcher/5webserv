#include "handlers/AHandler.hpp"
#include "utils/FileInfo.hpp"
#include "utils/log.hpp"

#include <sstream>

static const std::pair<HTTPStatus::_, std::string>    http_status_init_list[] =
{
    std::make_pair(HTTPStatus::SWITCHING_PROTOCOL, "101 Switching Protocol"),
    std::make_pair(HTTPStatus::CONTINUE, "100 Continue"),
    std::make_pair(HTTPStatus::OK, "200 OK"),
    std::make_pair(HTTPStatus::CREATED,	"201 Created"),
    std::make_pair(HTTPStatus::ACCEPTED, "202 Accepted"),
    std::make_pair(HTTPStatus::NON_AUTHORITATIVE_INFORMATION, "203 Non-Authoritative Information"),
    std::make_pair(HTTPStatus::NO_CONTENT, "204 No Content"),
    std::make_pair(HTTPStatus::RESET_CONTENT, "205 Reset Content"),
    std::make_pair(HTTPStatus::PARTIAL_CONTENT, "206 Partial Content"),
    std::make_pair(HTTPStatus::MULTIPLE_CHOICE, "300 Multiple Choice"),
    std::make_pair(HTTPStatus::MOVED_PERMANENTLY, "301 Moved Permanently"),
    std::make_pair(HTTPStatus::FOUND, "302 Found"),
    std::make_pair(HTTPStatus::SEE_OTHER, "303 See Other"),
    std::make_pair(HTTPStatus::NOT_MODIFIED, "304 Not Modified"),
    std::make_pair(HTTPStatus::USE_PROXY, "305 Use Proxy"),
    std::make_pair(HTTPStatus::SWITCH_PROXY, "306 Switch Proxy"),
    std::make_pair(HTTPStatus::TEMPORARY_REDIRECT, "307 Temporary Redirect"),
    std::make_pair(HTTPStatus::PERMANENT_REDIRECT, "308 Permanent Redirect"),
    std::make_pair(HTTPStatus::BAD_REQUEST, "400 Bad Request"),
    std::make_pair(HTTPStatus::UNAUTHORIZED, "401 Unauthorized"),
    std::make_pair(HTTPStatus::PAYMENT_REQUIRED, "402 Payment Required"),
    std::make_pair(HTTPStatus::FORBIDDEN, "403 Forbidden"),
    std::make_pair(HTTPStatus::NOT_FOUND, "404 Not Found"),
    std::make_pair(HTTPStatus::METHOD_NOT_ALLOWED, "405 Method Not Allowed"),
    std::make_pair(HTTPStatus::NOT_ACCEPTABLE, "406 Not Acceptable"),
    std::make_pair(HTTPStatus::PROXY_AUTHENTICATION_REQUIRED, "407 Proxy Authentication Required"),
    std::make_pair(HTTPStatus::REQUEST_TIMEOUT, "408 Request Timeout"),
    std::make_pair(HTTPStatus::CONFLICT, "409 Conflict"),
    std::make_pair(HTTPStatus::GONE, "410 Gone"),
    std::make_pair(HTTPStatus::LENGTH_REQUIRED, "411 Length Required"),
    std::make_pair(HTTPStatus::PRECONDITION_FAILED, "412 Precondition Failed"),
    std::make_pair(HTTPStatus::PAYLOAD_TOO_LARGE, "413 Payload Too Large"),
    std::make_pair(HTTPStatus::REQUEST_URI_TOO_LONG, "414 Request-URI Too Long"),
    std::make_pair(HTTPStatus::UNSUPPORTED_MEDIA_TYPE, "415 Unsupported Media Type"),
    std::make_pair(HTTPStatus::REQUESTED_RANGE_NOT_SATISFIABLE, "416 Requested Range Not Satisfiable"),
    std::make_pair(HTTPStatus::EXPECTATION_FAILED, "417 Expectation Failed"),
    std::make_pair(HTTPStatus::I_M_A_TEAPOT, "418 I'm a teapot"),
    std::make_pair(HTTPStatus::INTERNAL_SERVER_ERROR, "500 Internal Server Error"),
    std::make_pair(HTTPStatus::NOT_IMPLEMENTED, "501 Not Implemented"),
    std::make_pair(HTTPStatus::BAD_GATEWAY, "502 Bad Gateway"),
    std::make_pair(HTTPStatus::SERVICE_UNAVAILABLE, "503 Service Unavailable"),
    std::make_pair(HTTPStatus::GATEWAY_TIMEOUT, "504 Gateway Timeout"),
    std::make_pair(HTTPStatus::HTTP_VERSION_NOT_SUPPORTED, "505 HTTP Version Not Supported")
};

const std::map<HTTPStatus::_, std::string> AHandler::http_status_(http_status_init_list, http_status_init_list + sizeof(http_status_init_list) / sizeof(http_status_init_list[0]));

AHandler::AHandler(const Location& loc, const HTTPRequest& req) : location_(loc), request_(req)
{
    size_t  f;

    pure_uri_ = request_.uri();
    f = pure_uri_.find('?');
    if (f != std::string::npos)
    {
        query_string_ = pure_uri_.substr(f).erase(0, 1);
        pure_uri_.erase(f);
    }

    for (std::map<std::string, std::string>::const_iterator it = location_.cgi.begin(); it != location_.cgi.end(); it++)
    {
        f = pure_uri_.find('.' + it->first + '/');
        if (f != std::string::npos)
        {
            while (pure_uri_[f] != '/')
                ++f;
            path_info_ = pure_uri_.substr(f);
            pure_uri_.erase(f);
            break;
        }
    }
    normalizeUri(path_info_);
    normalizeUri(pure_uri_);

    logger::debug << "Handler: pure_uri=" << pure_uri_ <<\
        " query_string=" << query_string_ <<\
        " path_info=" << path_info_ << logger::end;
}

void    AHandler::normalizeUri(std::string &uri)
{
    size_t  percent = uri.find('%');

    while (percent != std::string::npos)
    {
        std::string         hex_value = uri.substr(percent + 1, 2);
        std::stringstream   converter(hex_value);
        int                 code;

        converter >> std::hex >> code;
        uri.replace(percent, 3, 1, code);
        percent = uri.find('%');
    }
}

void    AHandler::makeResponse(HTTPResponse& response)
{
    logger::info << "➡️  " << request_.method() << ' ' << request_.uri() << ' ' << request_.http() << logger::end;
    try
    {
        if (request_.getHeaderValue("Connection") == "close")
            response.addHeader("Connection", "close");
        if (location_.redirect.second.empty())
        {
            if (request_.http() != "HTTP/1.1")
                throw HTTPError(HTTPStatus::HTTP_VERSION_NOT_SUPPORTED);
            if (location_.methods.find(request_.method()) == location_.methods.end())
                throw HTTPError(HTTPStatus::METHOD_NOT_ALLOWED);

            this->handle(response);
        }
        else
            redirect(response);
    }
    catch(const HTTPError& e)
    {
        error(e.status(), response);
    }
    catch(const std::exception& e)
    {
        error(HTTPStatus::INTERNAL_SERVER_ERROR, response);
    }

    logger::info << "⬅️  "
        << response.buffer().substr(0, response.buffer().find("\r\n")) \
        << logger::end;
}

void    AHandler::error(HTTPStatus::_ status, HTTPResponse& response)
{
    response = HTTPResponse();

    std::string                                     body;
    const std::string&                              status_line = http_status_.find(status)->second;
    std::map<unsigned, std::string>::const_iterator it = location_.error_page.find(status);

    if (it != location_.error_page.end())
    {
        std::string     path = location_.root + '/' + it->second;
        std::ifstream   page_file(path.c_str(), std::ios::binary);
        FileInfo        page_file_info(path);

        if (page_file.is_open())
        {
            response.setContentType(page_file_info.type());
            std::noskipws(page_file);
            body.append(std::istream_iterator<char>(page_file), std::istream_iterator<char>());
        }
    }
    if (body.empty())
    {
        body += "<html>\n<head><meta charset=\"utf-8\"><title>";
        body += status_line;
        body += "</title></head>\n<body bgcolor=lightgray text=dimgray><center><h1>";
        body += status_line;
        body += "</h1></center><hr><center>webserv</center></body>\n</html>";
        response.setContentType("html");
    }
    response.setContentLength(body.length());
    response.buildResponse(body.begin(), body.end(), request_.method() != "HEAD", status_line);
}

void    AHandler::redirect(HTTPResponse& response)
{
    HTTPStatus::_       status = static_cast<HTTPStatus::_>(location_.redirect.first);
    const std::string   url = location_.redirect.second;

    response.addHeader("Location", url);
    response.setContentLength(0);
    response.buildResponse(NULL, NULL, false, http_status_.find(status)->second);
}
