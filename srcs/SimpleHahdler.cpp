#include "SimpleHandler.hpp"
#include "utils/string.hpp"

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
            path_ = location_.root + request_.uri();
            strRemoveDoubled(path_, '/');
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

    (void)response;
}

void    SimpleHandler::post(HTTPResponse&  response)
{

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
    body = "<html><head><title>";
    body += ss.str();
    body += "</title></head><body bgcolor=lightgray text=dimgray><center><h1>";
    body += ss.str();
    body += "</h1></center><hr><center>webserv</center></body></html>";

    response.setContentLength(body.length());
    response.setContentType("html");
    response.buildResponse(body.data(), body.length(), status);
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


// void addErrorPage(int code, const std::string& message, std::string& buffer)
// {
//     std::stringstream   ss;

//     ss << "💥" << code << ' ' << message;
//     buffer += "<html><head><title>";
//     buffer += ss.str();
//     buffer += "</title><body bgcolor=lightgray text=dimgray><center><h1>";
//     buffer += ss.str();
//     buffer += "</h1></center><hr><center>webserv</center></body></html>";
// }

// void addErrorPage(int code, const std::string& message, std::string& buffer)
// {
//     std::stringstream   ss;

//     ss << "💥" << code << ' ' << message;
//     buffer += "<html><head><title>";
//     buffer += ss.str();
//     buffer += "</title><body bgcolor=lightgray text=dimgray><center><h1>";
//     buffer += ss.str();
//     buffer += "</h1></center><hr><center>webserv</center></body></html>";
// }
