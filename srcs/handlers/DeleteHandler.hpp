#ifndef DELETEHANDLER_HPP
# define DELETEHANDLER_HPP

# include "handlers/AHandler.hpp"

class DeleteHandler : public AHandler
{
public:
    DeleteHandler(const Location& loc, const HTTPRequest& req) : AHandler(loc, req) {}

    virtual void    handle(HTTPResponse& response);
};

# endif
