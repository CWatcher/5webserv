#include "handlers/DeleteHandler.hpp"
#include "utils/FileInfo.hpp"

#include <unistd.h>

void    DeleteHandler::handle(HTTPResponse& response)
{
    FileInfo    file_info(location_.root + pure_uri_);
    int         ret;

    if (file_info.isNotExists())
        throw HTTPError(HTTPStatus::NOT_FOUND);

    if (file_info.isFile())
        ret = ::remove(file_info.path().c_str());
    else if (file_info.isDirectory())
        ret = ::rmdir(file_info.path().c_str());
    else
        throw HTTPError(HTTPStatus::FORBIDDEN);

    if (ret)
    {
        if (errno == EACCES)
            throw HTTPError(HTTPStatus::FORBIDDEN);
        if (errno == ENOTEMPTY)
            throw HTTPError(HTTPStatus::CONFLICT);
        throw HTTPError(HTTPStatus::INTERNAL_SERVER_ERROR);
    }

    response.buildResponse(NULL, NULL, http_status_.find(HTTPStatus::NO_CONTENT)->second);
}
