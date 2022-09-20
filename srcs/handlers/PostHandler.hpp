#ifndef POSTHANDLER_HPP
# define POSTHANDLER_HPP

# include "handlers/ACgiHandler.hpp"

class PostHandler : public ACgiHandler
{
public:
    PostHandler(const Location& loc, const HTTPRequest& req, in_addr_t s_ip, in_port_t s_port, const in_addr& remote_addr) :
    ACgiHandler(loc, req, s_ip, s_port, remote_addr) {}

    virtual void    handle(HTTPResponse& response);

private:
    void            postFile(HTTPResponse& response) const;
    std::string     getFormFileName(size_t form_header_first, size_t form_header_last) const;
};

#endif
