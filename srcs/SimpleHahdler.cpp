#include "SimpleHandler.hpp"
#include "utils/string.hpp"
#include "utils/stat.hpp"
#include "dirent.h"

#include <sstream>

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
    std::stringstream   ss;
    std::string         body;

    ss << "💥" << status << ' ' << HTTPResponse::http_status_msg.find(status)->second;
    body = "<html>\n<head><title>";
    body += ss.str();
    body += "</title></head>\n<body bgcolor=lightgray text=dimgray><center><h1>";
    body += ss.str();
    body += "</h1></center><hr><center>webserv</center></body>\n</html>";

    response.setContentLength(body.length());
    response.setContentType("html");
    response.buildResponse(body.begin(), body.end(), status);
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
    body += "</h1>\n<hr><table><tr><th>name</th><th allign=right>last modification</th><th allign=left>size</th></tr>";

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
