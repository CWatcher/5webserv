#ifndef ACGIHANDLER_HPP
# define ACGIHANDLER_HPP

# include "handlers/AHandler.hpp"
# include "utils/FileInfo.hpp"

class ACgiHandler : public AHandler
{
public:
    ACgiHandler(const Location& loc, const HTTPRequest& req, in_addr_t s_ip, in_port_t s_port, const in_addr& remote_addr);

private:
    virtual void        handle(HTTPResponse& response) = 0;

protected:
    const in_addr_t     server_ip_;
    const in_port_t     server_port_;
    const in_addr&      remote_addr_;
    FileInfo            file_info_;
    std::string         cgi_path_;
};

#endif
