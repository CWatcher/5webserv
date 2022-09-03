#include "handlers/GetHandler.hpp"

#include <dirent.h>

void GetHandler::handle(HTTPResponse &response)
{
    if (file_info_.isNotExists())
        throw HTTPError(HTTPStatus::NOT_FOUND);
    if (!file_info_.isReadble())
        throw HTTPError(HTTPStatus::FORBIDDEN);

    if (file_info_.isFile())
        getFile(response);
    else if (file_info_.isDirectory())
        getDirectory(response);
    else
        throw HTTPError(HTTPStatus::FORBIDDEN);
}

void    GetHandler::getFile(HTTPResponse& response) const
{
    std::ifstream   file;

    if (!cgi_path.empty())
    {
        // cgi(response);
        return;
    }

    file.open(file_info_.path().c_str(), std::ios::binary);
    if (!file.is_open())
        throw HTTPError(HTTPStatus::INTERNAL_SERVER_ERROR);

    response.setContentType(file_info_.type());
    response.setContentLength(file_info_.size());

    std::noskipws(file);
    if (file_info_.size() == 0)
        response.buildResponse(std::istream_iterator<char>(file), std::istream_iterator<char>(), http_status_.find(HTTPStatus::NO_CONTENT)->second);
    else
        response.buildResponse(std::istream_iterator<char>(file), std::istream_iterator<char>());
}

void    GetHandler::getDirectory(HTTPResponse& response)
{
    if (*pure_uri_.rbegin() != '/')
        pure_uri_.push_back('/');
    for (std::vector<std::string>::const_iterator file = location_.index.begin(); file != location_.index.end(); ++file)
    {
        FileInfo    index_file_info_ = FileInfo(location_.root + pure_uri_ + *file);
        if (index_file_info_.isFile() && index_file_info_.isReadble())
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
    throw HTTPError(HTTPStatus::FORBIDDEN);
}

void    GetHandler::getAutoindex(HTTPResponse& response) const
{
    std::string body;
    DIR*        dir = ::opendir(file_info_.path().c_str());
    dirent*     item;

    if (dir == NULL)
        throw HTTPError(HTTPStatus::INTERNAL_SERVER_ERROR);

    body += "<html>\n<head><meta charset=\"utf-8\"><style>td{padding-right: 3em}th{text-align: left;}</style><title>📁";
    body += pure_uri_;
    body += "</title></head>\n<body bgcolor=lightgray text=dimgray><h1>📁";
    body += pure_uri_;
    body += "</h1><hr>\n<table><tr><th>name</th><th allign=right>last modification</th><th allign=left>size</th></tr>";

    while ((item = ::readdir(dir)) != NULL)
    {
        std::string item_name(item->d_name);
        FileInfo    item_info(file_info_.path() + item_name);

        if (item->d_type & DT_DIR)
            item_name += '/';
        body += "\n<tr><td><pre>";
        body += item->d_type & DT_DIR ? "📁" : "📄";
        body += "<a href=\"";
        body += pure_uri_ + item_name;
        body += "\">";
        body += item_name;
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
