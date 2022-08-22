#include "SimpleHandler.hpp"
#include "utils/string.hpp"
#include "utils/log.hpp"

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

SimpleHandler::SimpleHandler(const Location& loc, const HTTPRequest& req) : location_(loc), request_(req)
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
            break;
        }
    }

    file_info_ = FileInfo(location_.root + pure_uri_);

    logger::debug << "SimpleHandler: uri=" << pure_uri_ <<\
        " query_string=" << query_string_ <<\
        " path_info=" << path_info_ <<\
        " path=" << file_info_.path() << logger::end;
}

void    SimpleHandler::fillResponse(HTTPResponse& response)
{
    try
    {
        if (location_.redirect.second.empty())
        {
            // Проверка корректности запроса
            if (request_.method().empty() && request_.uri().empty() && request_.http().empty())
                throw SimpleHandler::HTTPError(BAD_REQUEST);
            if (request_.http() != "HTTP/1.1")
                throw SimpleHandler::HTTPError(HTTP_VERSION_NOT_SUPPORTED);
            //TO DO проверить заголовки, host должен быть обязательно? что ещё нужно проверить?
                // throw SimpleHandler::HTTPError(BAD_REQUEST);

            std::map<std::string, SimpleHandler::handler>::const_iterator handler = handlers_.find(request_.method());
            if (handlers_.find(request_.method()) == handlers_.end())
                throw SimpleHandler::HTTPError(NOT_IMPLEMENTED);
            if (location_.methods.find(request_.method()) == location_.methods.end())
                throw SimpleHandler::HTTPError(METHOD_NOT_ALLOWED);
            (this->*handler->second)(response);
        }
        else
            redirect(location_.redirect.first, response);
    }
    catch(const SimpleHandler::HTTPError& e)
    {
        logger::debug << "SimpleHandler: " << http_status_.find(e.status())->second << logger::end;
        error(e.status(), response);
    }
    catch(const std::exception& e)
    {
        logger::error << "SimpleHandler: " << logger::cerror << logger::end;
        error(INTERNAL_SERVER_ERROR, response);
    }
}

void    SimpleHandler::get(HTTPResponse&  response)
{
    logger::debug << "SimpleHandler: GET " << file_info_.path() << logger::end;

    if (!file_info_.isExists())
        throw SimpleHandler::HTTPError(NOT_FOUND);
    if (!file_info_.isReadble())
        throw SimpleHandler::HTTPError(FORBIDDEN);

    if (file_info_.isFile())
        getFile(response);
    else if (file_info_.isDirectory())
        getDirectory(response);
    else
        throw SimpleHandler::HTTPError(NOT_FOUND);
}

void    SimpleHandler::getFile(HTTPResponse& response)
{
    std::map<std::string, std::string>::const_iterator  cgi = location_.cgi.find(file_info_.type());
    std::ifstream                                       file(file_info_.path().c_str(), std::ifstream::binary);

    if (cgi != location_.cgi.end())
    {
        cgiHandler(response);
        return;
    }

    if (!file.is_open())
        throw SimpleHandler::HTTPError(INTERNAL_SERVER_ERROR);

    response.setContentType(file_info_.type());
    response.setContentLength(file_info_.size());

    std::noskipws(file);
    if (file_info_.size() == 0)
        response.buildResponse(std::istream_iterator<char>(file), std::istream_iterator<char>(), "204 No Content");
    else
        response.buildResponse(std::istream_iterator<char>(file), std::istream_iterator<char>());
}

void    SimpleHandler::getDirectory(HTTPResponse& response)
{
    for (std::vector<std::string>::const_iterator file = location_.index.begin(); file != location_.index.end(); ++file)
    {
        FileInfo    index_file_info_ = FileInfo(location_.root + '/' + *file);
        if (index_file_info_.isExists() && index_file_info_.isFile() && index_file_info_.isReadble())
        {
            file_info_ = index_file_info_;
            getFile(response);
            return;
        }
    }
    if (location_.autoindex)
    {
        getAutoindex(response);
        return;
    }
    throw SimpleHandler::HTTPError(FORBIDDEN);
}

void    SimpleHandler::getAutoindex(HTTPResponse& response)
{
    std::string body;
    std::string path = file_info_.path();
    DIR*        dir = ::opendir(file_info_.path().c_str());
    dirent*     item;

    if (dir == NULL)
        throw SimpleHandler::HTTPError(INTERNAL_SERVER_ERROR);

    strCompleteWith(pure_uri_, '/');
    body += "<html>\n<head><style>td{padding-right: 3em}th{text-align: left;}</style><title>📁";
    body += pure_uri_;
    body += "</title></head>\n<body bgcolor=lightgray text=dimgray><h1>📁";
    body += pure_uri_;
    body += "</h1><hr>\n<table><tr><th>name</th><th allign=right>last modification</th><th allign=left>size</th></tr>";

    strCompleteWith(path, '/');
    while ((item = ::readdir(dir)) != NULL)
    {
        std::string name(item->d_name);
        FileInfo    item_info(path + name);

        if (item->d_type & DT_DIR)
            name += '/';
        body += "\n<tr><td><pre>";
        body += item->d_type & DT_DIR ? "📁" : "📄";
        body += "<a href=\"";
        body += pure_uri_ + name;
        body += "\">";
        body += name;
        body += "</a></pre></td><td><pre>";
        body += item_info.dateStr();
        body += "<pre></td><td><pre>";
        body += item_info.sizeStr();
        body += "<pre></td></tr>";
    }
    ::closedir(dir);
    body += "\n</table><hr>\n</body>\n</html>";

    response.setContentLength(body.length());
    response.setContentType("html");
    response.buildResponse(body.begin(), body.end());
}

void    SimpleHandler::post(HTTPResponse&  response)
{
    //проверить body size
    logger::debug << "SimpleHandler: POST " << file_info_.path() << logger::end;
    (void)response;
}

void    SimpleHandler::del(HTTPResponse&  response)
{
    logger::debug << "SimpleHandler: DELETE " << file_info_.path() << logger::end;
    (void)response;
}

void    SimpleHandler::error(HTTPStatus status, HTTPResponse& response)
{
    std::string                                     body;
    const std::string&                              status_line = http_status_.find(status)->second;
    std::map<unsigned, std::string>::const_iterator it = location_.error_page.find(status);

    if (it != location_.error_page.end())
    {
        std::string     path = location_.root + '/' + it->second;
        std::ifstream   page_file(path.c_str(), std::ifstream::binary);
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
        body += "<html>\n<head><title>";
        body += status_line;
        body += "</title></head>\n<body bgcolor=lightgray text=dimgray><center><h1>";
        body += status_line;
        body += "</h1></center><hr><center>webserv</center></body>\n</html>";
        response.setContentType("html");
    }
    response.setContentLength(body.length());
    response.buildResponse(body.begin(), body.end(), status_line);
}

void    SimpleHandler::redirect(unsigned status, HTTPResponse& response)
{
    (void) response;
    (void) status;
}

void    SimpleHandler::cgiHandler(HTTPResponse& response)
{
    (void) response;
    logger::debug << "CGI for ." << file_info_.type() << logger::end;
}
