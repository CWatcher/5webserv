#ifndef ACGIHANDLER_HPP
# define ACGIHANDLER_HPP

# include "handlers/AHandler.hpp"
# include "utils/FileInfo.hpp"

class ACgiHandler : public AHandler
{
public:
    ACgiHandler(const Location& loc, const HTTPRequest& req, in_addr_t s_ip, in_port_t s_port, const in_addr& remote_addr);

protected:
    virtual void        handle(HTTPResponse& response) = 0;
    void                cgi(HTTPResponse& response) const;
    void                runCgi(FILE* cgi_out_file) const;
    void                waitCgi(pid_t cgi_pid, FILE* cgi_out_file, HTTPResponse& response) const;
    void                makeCgiEnv(std::vector<char*>& envp_data) const;
    void                makeCgiResponse(HTTPResponse& response, const char* cgi_data, size_t n) const;

protected:
    const in_addr_t     server_ip_;
    const in_port_t     server_port_;
    const in_addr&      remote_addr_;
    FileInfo            file_info_;
    std::string         cgi_path_;
};

#endif
