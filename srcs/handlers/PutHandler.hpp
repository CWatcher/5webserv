#ifndef PUTHANDLER_HPP
#define PUTHANDLER_HPP

# include "handlers/AHandler.hpp"

class PutHandler : public AHandler
{
public:
    PutHandler(const Location& loc, const HTTPRequest& req) : AHandler(loc, req) {}

    virtual void	handle(HTTPResponse& response);
};

#endif
