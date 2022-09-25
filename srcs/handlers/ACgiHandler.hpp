#ifndef ACGIHANDLER_HPP
# define ACGIHANDLER_HPP

# include "handlers/AHandler.hpp"
# include "utils/FileInfo.hpp"

class ACgiHandler : public AHandler
{
public:
    ACgiHandler(const Location& loc, const HTTPRequest& req, in_addr_t s_ip, in_port_t s_port, const in_addr& remote_addr);
    virtual ~ACgiHandler();

protected:
    void                runCgi(HTTPResponse& response);
    void                forkCgi() const;
    void                makeCgiEnv(std::vector<char*>& envp_data) const;
    void                parentCgi(HTTPResponse& response) const;
    void                makeCgiResponse(const char* cgi_data, size_t n, HTTPResponse& response) const;
    static const char*  getCgiBody(const char* cgi_data, size_t n);

private:
    virtual void        handle(HTTPResponse& response) = 0;

protected:
    FileInfo            file_info_;
    std::string         cgi_path_;
    pid_t               cgi_pid_;

private:
    const in_addr_t     server_ip_;
    const in_port_t     server_port_;
    const in_addr&      remote_addr_;

    FILE*               cgi_in_file_;
    FILE*               cgi_out_file_;
};

#endif
