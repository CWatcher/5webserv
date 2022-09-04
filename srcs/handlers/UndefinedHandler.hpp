#ifndef UNDEFINEDHANDLER_HPP
# define UNDEFINEDHANDLER_HPP

# include "handlers/AHandler.hpp"

class UndefinedHandler : public AHandler
{
public:
    UndefinedHandler(const Location& loc, const HTTPRequest& req) : AHandler(loc, req) {}

private:
    virtual void    handle(HTTPResponse&) {throw HTTPError(HTTPStatus::NOT_IMPLEMENTED);}
};

#endif
