#include "UndefinedHandler.hpp"
#include "utils/log.hpp"

void    UndefinedHandler::makeResponse(HTTPResponse& response)
{
    logger::info << "➡️  " << request_.method() << ' ' << request_.uri() << ' ' << request_.http() << logger::end;

    error(HTTPStatus::NOT_IMPLEMENTED, response);

    logger::info << "⬅️  "
        << response.buffer().substr(0, response.buffer().find("\r\n")) \
        << logger::end;
}
