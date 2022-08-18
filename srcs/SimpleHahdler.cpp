#include "SimpleHandler.hpp"
#include "utils/string.hpp"
#include "utils/stat.hpp"
#include "dirent.h"

#include <sstream>

static const std::pair<HTTPStatus, std::string>    http_status_init_list[] =
{
    std::make_pair(SWITCHING_PROTOCOL, "101 Switching Protocol"),
    std::make_pair(CONTINUE, "100 Continue"),
    std::make_pair(PROCESSING, "102 Processing"),
    std::make_pair(EARLY_HINTS,	"103 Early Hints"),
    std::make_pair(OK, "200 OK"),
    std::make_pair(CREATED,	"201 Created"),
    std::make_pair(ACCEPTED, "202 Accepted"),
    std::make_pair(NON_AUTHORITATIVE_INFORMATION, "203 Non-Authoritative Information"),
    std::make_pair(NO_CONTENT, "204 No Content"),
    std::make_pair(RESET_CONTENT, "205 Reset Content"),
    std::make_pair(PARTIAL_CONTENT, "206 Partial Content"),
    std::make_pair(MULTIPLE_CHOICE, "300 Multiple Choice"),
    std::make_pair(MOVED_PERMANENTLY, "301 Moved Permanently"),
    std::make_pair(FOUND, "302 Found"),
    std::make_pair(SEE_OTHER, "303 See Other"),
    std::make_pair(NOT_MODIFIED, "304 Not Modified"),
    std::make_pair(USE_PROXY, "305 Use Proxy"),
    std::make_pair(SWITCH_PROXY, "306 Switch Proxy"),
    std::make_pair(TEMPORARY_REDIRECT, "307 Temporary Redirect"),
    std::make_pair(PERMANENT_REDIRECT, "308 Permanent Redirect"),
    std::make_pair(BAD_REQUEST, "400 Bad Request"),
    std::make_pair(UNAUTHORIZED, "401 Unauthorized"),
    std::make_pair(PAYMENT_REQUIRED, "402 Payment Required"),
    std::make_pair(FORBIDDEN, "403 Forbidden"),
    std::make_pair(NOT_FOUND, "404 Not Found"),
    std::make_pair(METHOD_NOT_ALLOWED, "405 Method Not Allowed"),
    std::make_pair(NOT_ACCEPTABLE, "406 Not Acceptable"),
    std::make_pair(PROXY_AUTHENTICATION_REQUIRED, "407 Proxy Authentication Required"),
    std::make_pair(REQUEST_TIMEOUT, "408 Request Timeout"),
    std::make_pair(CONFLICT, "409 Conflict"),
    std::make_pair(GONE, "410 Gone"),
    std::make_pair(LENGTH_REQUIRED, "411 Length Required"),
    std::make_pair(PRECONDITION_FAILED, "412 Precondition Failed"),
    std::make_pair(REQUEST_ENTITY_TOO_LARGE, "413 Request Entity Too Large"),
    std::make_pair(REQUEST_URI_TOO_LONG, "414 Request-URI Too Long"),
    std::make_pair(UNSUPPORTED_MEDIA_TYPE, "415 Unsupported Media Type"),
    std::make_pair(REQUESTED_RANGE_NOT_SATISFIABLE, "416 Requested Range Not Satisfiable"),
    std::make_pair(EXPECTATION_FAILED, "417 Expectation Failed"),
    std::make_pair(INTERNAL_SERVER_ERROR, "500 Internal Server Error"),
    std::make_pair(NOT_IMPLEMENTED, "501 Not Implemented"),
    std::make_pair(BAD_GATEWAY, "502 Bad Gateway"),
    std::make_pair(SERVICE_UNAVAILABLE, "503 Service Unavailable"),
    std::make_pair(GATEWAY_TIMEOUT, "504 Gateway Timeout"),
    std::make_pair(HTTP_VERSION_NOT_SUPPORTED, "505 HTTP Version Not Supported")
};

const std::map<HTTPStatus, std::string> SimpleHandler::http_status_(http_status_init_list, http_status_init_list + sizeof(http_status_init_list) / sizeof(http_status_init_list[0]));


const std::pair<std::string, SimpleHandler::handler>    SimpleHandler::handlers_init_list_[] =
{
    std::make_pair("GET", &SimpleHandler::get),
    std::make_pair("POST", &SimpleHandler::post),
    std::make_pair("DELETE", &SimpleHandler::del),
};

const std::map<std::string, SimpleHandler::handler> SimpleHandler::handlers_(handlers_init_list_, handlers_init_list_ + sizeof(handlers_init_list_)/ sizeof(handlers_init_list_[0]));

void    SimpleHandler::fillResponse(HTTPResponse& response)
{
    try
    {
        validateRequest();
        if (location_.redirect.second.empty())
        {
            //распарсить uri, пока считаю что там ничего лишнего кроме пути нет
            path_ = location_.root + request_.uri();
            file_name_ = path_.substr(path_.rfind('/'));
            size_t t = file_name_.rfind('.');
            if (t != std::string::npos)
                type_ = file_name_.substr(t + 1);

            std::map<std::string, SimpleHandler::handler>::const_iterator handler = handlers_.find(request_.method());
            (this->*handler->second)(response);
        }
        else
            redirect(location_.redirect.first, response);
    }
    catch(const SimpleHandler::HTTPError& e)
    {
        error(e.status(), response);
    }
    catch(const std::exception& e)
    {
        error(INTERNAL_SERVER_ERROR, response);
    }
}

void    SimpleHandler::validateRequest()
{
    if (request_.method().empty() && request_.uri().empty() && request_.http().empty())
        throw SimpleHandler::HTTPError(BAD_REQUEST);
    if (request_.http() != "HTTP/1.1")
        throw SimpleHandler::HTTPError(HTTP_VERSION_NOT_SUPPORTED);
    if (handlers_.find(request_.method()) == handlers_.end())
        throw SimpleHandler::HTTPError(NOT_IMPLEMENTED);
    if (location_.methods.find(request_.method()) == location_.methods.end())
        throw SimpleHandler::HTTPError(METHOD_NOT_ALLOWED);
    //TO DO проверить заголовки
    //host должен быть обязательно??
        // throw SimpleHandler::HTTPError(BAD_REQUEST);
}

void    SimpleHandler::get(HTTPResponse&  response)
{
    ft::stat    file_stat(path_);

    if (!file_stat.isExists())
        throw SimpleHandler::HTTPError(NOT_FOUND);
    if (!file_stat.isReadble())
        throw SimpleHandler::HTTPError(FORBIDDEN);
    if (file_stat.isFile())
        getFile(response);
    else if (file_stat.isDirectory() && location_.autoindex)
        getAutoindex(response);
    else
        throw SimpleHandler::HTTPError(NOT_FOUND);
}

void    SimpleHandler::getFile(HTTPResponse& response)
{
    std::ifstream   file(path_.c_str(), std::ifstream::binary);
    ft::stat        f_stat(path_);

    if (!file.is_open())
        throw SimpleHandler::HTTPError(INTERNAL_SERVER_ERROR);

    response.setContentType(type_);
    response.setContentLength(f_stat.size());
    std::noskipws(file);
    response.buildResponse(std::istream_iterator<char>(file), std::istream_iterator<char>());
}

void    SimpleHandler::post(HTTPResponse&  response)
{
    //проверить body size
    (void)response;
}

void    SimpleHandler::del(HTTPResponse&  response)
{
    (void)response;
}

void    SimpleHandler::error(HTTPStatus status, HTTPResponse& response)
{
    // TO Do если есть кастомная ошибка
    //иначе
    const std::string&  status_line = http_status_.find(status)->second;
    std::string         body;

    body += "<html>\n<head><title>";
    body += status_line;
    body += "</title></head>\n<body bgcolor=lightgray text=dimgray><center><h1>";
    body += status_line;
    body += "</h1></center><hr><center>webserv</center></body>\n</html>";

    response.setContentLength(body.length());
    response.setContentType("html");
    response.buildResponse(body.begin(), body.end(), status_line);
}

void    SimpleHandler::getAutoindex(HTTPResponse& response)
{
    std::string relative_path = request_.uri();
    DIR*        dir = ::opendir(path_.c_str());
    dirent*     item;
    std::string body;

    if (dir == NULL)
        throw SimpleHandler::HTTPError(INTERNAL_SERVER_ERROR);
    strCompleteWith(path_, '/');
    strCompleteWith(relative_path, '/');
    body += "<html>\n<head><style>td{padding-right: 3em}th{text-align: left;}</style><title>📁";
    body += relative_path;
    body += "</title></head>\n<body bgcolor=lightgray text=dimgray><h1>📁";
    body += relative_path;
    body += "</h1><hr>\n<table><tr><th>name</th><th allign=right>last modification</th><th allign=left>size</th></tr>";

    while ((item = ::readdir(dir)) != NULL)
    {
        std::string name(item->d_name);
        ft::stat    f_stat(path_ + name);

        if (item->d_type & DT_DIR)
            name += '/';
        body += "\n<tr><td><pre>";
        body += item->d_type & DT_DIR ? "📁" : "📄";
        body += "<a href=\"";
        body += relative_path + name;
        body += "\">";
        body += name;
        body += "</a></pre></td><td><pre>";
        body += f_stat.strDate();
        body += "<pre></td><td><pre>";
        body += f_stat.strSize();
        body += "<pre></td></tr>";
    }
    ::closedir(dir);
    body += "\n</table><hr>\n</body>\n</html>";

    response.setContentLength(body.length());
    response.setContentType("html");
    response.buildResponse(body.begin(), body.end());
}

void    SimpleHandler::redirect(unsigned status, HTTPResponse& response)
{
    (void) response;
    (void) status;
}
