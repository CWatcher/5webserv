#include "handlers/ACgiHandler.hpp"

ACgiHandler::ACgiHandler(const Location& loc, const HTTPRequest& req, in_addr_t s_ip, in_port_t s_port, const in_addr& remote_addr) :
    AHandler(loc, req),
    server_ip_(s_ip),
    server_port_(s_port),
    remote_addr_(remote_addr),
    file_info_(location_.root + pure_uri_)
{
    std::map<std::string, std::string>::const_iterator  cgi = location_.cgi.find(file_info_.type());

    if (cgi != location_.cgi.end())
        cgi_path_ = cgi->second;
}
