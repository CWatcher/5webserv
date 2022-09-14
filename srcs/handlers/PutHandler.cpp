#include "handlers/PutHandler.hpp"
#include "utils/FileInfo.hpp"

void    PutHandler::handle(HTTPResponse&)
{
    FileInfo        file_info(location_.root + pure_uri_);
    bool            new_file_created = file_info.isNotExists();
    std::ofstream   file;

    file.open(file_info.path().c_str());
    if (!file.is_open())
        throw HTTPError(HTTPStatus::FORBIDDEN);
    file.write(request_.body(), request_.body_size());
    if (!file.good())
        throw HTTPError(HTTPStatus::INTERNAL_SERVER_ERROR);

    if (new_file_created)
        throw HTTPError(HTTPStatus::CREATED);
    else
        throw HTTPError(HTTPStatus::OK);
}
