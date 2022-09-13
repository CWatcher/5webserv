#include "handlers/PostHandler.hpp"

void    PostHandler::handle(HTTPResponse&  response)
{
    if (location_.body_size != 0 && request_.body_size() > location_.body_size)
        throw HTTPError(HTTPStatus::PAYLOAD_TOO_LARGE);
    if (request_.body_size() == 0)
        throw HTTPError(HTTPStatus::NO_CONTENT);

    if (!cgi_path_.empty())
    {
        if (file_info_.isNotExists())
            throw HTTPError(HTTPStatus::NOT_FOUND);
        if (!file_info_.isReadble() || !file_info_.isFile())
            throw HTTPError(HTTPStatus::FORBIDDEN);
        cgi(response);
    }
    else if (request_.isFormData())
    {
        if (pure_uri_ != location_.path && pure_uri_ != location_.path + "/")
            throw HTTPError(HTTPStatus::NOT_FOUND);
        postFile(response);
    }
    else
        throw HTTPError(HTTPStatus::BAD_REQUEST);
}

void    PostHandler::postFile(HTTPResponse& response) const
{
    std::ofstream   new_file;
    std::string     filename;
    std::string     message;
    std::string     boundary = request_.getHeaderParameter("Content-Type", "boundary").insert(0, "--");
    size_t          form_data_start = request_.raw_data().find("\r\n\r\n", request_.body_offset());
    size_t          form_data_end = request_.raw_data().find(boundary, form_data_start);

    if (boundary.length() == 2 || form_data_start == std::string::npos || form_data_end == std::string::npos)
        throw HTTPError(HTTPStatus::BAD_REQUEST);

    filename = getFormFileName(request_.body_offset() + boundary.size() + 2, form_data_start);
    if (FileInfo(location_.upload_store + "/" + filename).isNotExists() == false)
        throw HTTPError(HTTPStatus::CONFLICT);

    new_file.open((location_.upload_store + "/" + filename).c_str(), std::ios::binary);
    if (!new_file.is_open())
        throw HTTPError(HTTPStatus::FORBIDDEN);

    form_data_start += 4;
    form_data_end -= 2;
    new_file.write(request_.raw_data().data() + form_data_start, form_data_end - form_data_start);
    if (!new_file.good())
    {
        new_file.close();
        throw HTTPError(HTTPStatus::INTERNAL_SERVER_ERROR);
    }
    new_file.close();

    message +="<html>\n<head><meta charset=\"utf-8\"><title>";
    message += filename;
    message += "</title></head>\n<body bgcolor=lightgray text=dimgray><center><h1>";
    message += filename;
    message += " uploaded </h1></center><hr><center>webserv</center></body>\n</html>";

    response.setContentLength(message.length());
    response.setContentType("html");
    response.addHeader("Location", pure_uri_ + (*pure_uri_.rbegin() == '/' ? "" : "/") + filename);
    response.buildResponse(message.begin(), message.end(), true, http_status_.find(HTTPStatus::CREATED)->second);
}

std::string     PostHandler::getFormFileName(size_t form_header_first, size_t form_header_last) const
{
    const std::string   form_header = request_.raw_data().substr(form_header_first, form_header_last - form_header_first);
    size_t              f = form_header.find("filename=\"");
    std::string         filename;

    if (f != std::string::npos)
        try
        {
            size_t  first = f + 10;
            size_t  last = form_header.find("\"", first);

            filename = form_header.substr(first, last - first);
        }
        catch (std::out_of_range&)
        {
            throw HTTPError(HTTPStatus::BAD_REQUEST);
        }

    if (filename.empty())
        throw HTTPError(HTTPStatus::BAD_REQUEST);
    return filename;
}
