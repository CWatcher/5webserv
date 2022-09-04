#ifndef GETHANDLER_HPP
# define GETHANDLER_HPP

# include "handlers/ACgiHandler.hpp"

class GetHandler : public ACgiHandler
{
public:
    GetHandler(const Location& loc, const HTTPRequest& req, in_addr_t s_ip, in_port_t s_port, const in_addr& remote_addr) :
    ACgiHandler(loc, req, s_ip, s_port, remote_addr) {}

private:
    virtual void    handle(HTTPResponse& response);
    void            getFile(HTTPResponse& response) const;
    void            getDirectory(HTTPResponse& response);
    void            getAutoindex(HTTPResponse& response) const;
};

#endif
