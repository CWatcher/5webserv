#include "SimpleHandler.hpp"
#include "utils/string.hpp"
#include "utils/stat.hpp"
#include "dirent.h"

#include <sstream>
// #include <iostream> ///
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
        // throw SimpleHandler::HTTPError(METHOD_NOT_ALLOWED);
        if (location_.redirect.second.empty())
        {
            path_ = location_.root + request_.uri();
            strRemoveDoubled(path_, '/');
            // std::cout << path_ << "!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
            //распарсить uri
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
    //host должен быть??
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
        readFile(response);
    else if (file_stat.isDirectory() && location_.autoindex)
        autoindex(response);
    else
        throw SimpleHandler::HTTPError(NOT_FOUND);
}

void    SimpleHandler::readFile(HTTPResponse& response)
{
    std::ifstream   file(path_.c_str());

    if (!file.is_open())
        throw SimpleHandler::HTTPError(INTERNAL_SERVER_ERROR);
    (void)response;
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

    response.setContentType("html");
    response.buildResponse(body, status);
}

void    SimpleHandler::autoindex(HTTPResponse& response)
{
    std::string relative_path = request_.uri();
    DIR*        dir = ::opendir(path_.c_str());
    dirent*     item;
    std::string body;

    if (dir == NULL)
        throw SimpleHandler::HTTPError(INTERNAL_SERVER_ERROR);

    if (*--relative_path.end() != '/')
        relative_path += '/';
    body += "<html>\n<head><style>td{padding-right: 3em}th{text-align: left;}</style><title>📁";
    body += relative_path;
    body += "</title></head>\n<body bgcolor=lightgray text=dimgray><h1>📁";
    body += relative_path;
    body += "</h1>\n<hr><table><tr><th>name</th><th allign=right>last modification</th><th allign=left>size</th></tr>";

    while ((item = ::readdir(dir)) != NULL)
    {
        std::string name(item->d_name);
        ft::stat    f_stat(path_);

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
    body += "\n</table><hr>\n</body>\n</html>";
    ::closedir(dir);

    response.setContentType("html");
    response.buildResponse(body);
}

void    SimpleHandler::redirect(unsigned status, HTTPResponse& response)
{
    (void) response;
    (void) status;
}



// class FileInfo
// {
// public:
//     FileInfo(const std::string& path, const std::string& name) {::lstat((path + '/' + name).c_str(), &info_);}

//     std::string getDate() const;
//     std::string getSize() const;
// private:
//     struct stat info_;
// };

// std::string FileInfo::getDate() const
// {
//     char    date[21];
//     strftime(date, 21, "%d-%h-%Y %R", gmtime(&info_.st_mtim.tv_sec));
//     return date;
// }

// std::string FileInfo::getSize() const
// {
//     std::stringstream   ss;

//     if (S_ISREG(info_.st_mode))
//     {
//         ss << info_.st_size;
//         return ss.str();
//     }
//     return "-";
// }

// void addAutoindex(std::string& autoindex_html, const std::string& path)
// {
//     autoindex_html += "<html><head><style>td{padding-right: 3em}th{text-align: left;}</style><title>📁";
//     autoindex_html += path + '/';
//     autoindex_html += "</title></head><body bgcolor=lightgray text=dimgray><h1>📁";
//     autoindex_html += path + '/';
//     autoindex_html += "<hr><table>";
//     autoindex_html += "<tr><th>name</th><th allign=right>last modification</th><th allign=left>size</th></tr>";

//     DIR*        dir = ::opendir(path.c_str());
//     dirent*     item;
//     if (dir)
//     {
//         while ((item = ::readdir(dir)) != NULL)
//         {
//             std::string name(item->d_name);
//             FileInfo file(path, name);
//             if (item->d_type & DT_DIR)
//                 name += '/';
//             autoindex_html += "<tr><td><pre>";
//             autoindex_html += item->d_type & DT_DIR ? "📁" : "📄";
//             autoindex_html += "<a href=\"";
//             autoindex_html += path + '/' + name;
//             // autoindex_html += name;
//             autoindex_html += "\">";
//             autoindex_html += name;
//             autoindex_html += "</a></pre></td><td><pre>";
//             autoindex_html += file.getDate();
//             autoindex_html += "<pre></td><td><pre>";
//             autoindex_html += file.getSize();
//             autoindex_html += "<pre></td></tr>";
//         }
//         ::closedir(dir);
//     }
//     autoindex_html.append("</table><hr></body></html>");
// }
