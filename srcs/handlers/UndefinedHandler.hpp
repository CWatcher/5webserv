#ifndef UNDEFINEDHANDLER_HPP
# define UNDEFINEDHANDLER_HPP

# include "handlers/AHandler.hpp"

class UndefinedHandler : public AHandler
{
public:
    UndefinedHandler(const Location& loc, const HTTPRequest& req) : AHandler(loc, req) {}

    virtual void    makeResponse(HTTPResponse& request);
private:
    void            handle(HTTPResponse&) {}
};

#endif
