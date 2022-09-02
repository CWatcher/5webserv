#ifndef SIMPLEHANDLER_HPP
# define SIMPLEHANDLER_HPP

# include "http/HTTPResponse.hpp"
# include "http/HTTPRequest.hpp"
# include "http/HTTPStatus.hpp"
# include "config/ServerConfig.hpp"
# include "utils/FileInfo.hpp"

class SimpleHandler
{
public:
    SimpleHandler(const Location& loc, const HTTPRequest& req, in_addr_t server_ip, in_port_t server_port, const in_addr& remote_addr);

    void                makeResponse(HTTPResponse& response);

private:
    void                get(HTTPResponse& response);
    void                post(HTTPResponse& response);
    void                del(HTTPResponse& response);

    void                error(HTTPStatus status, HTTPResponse& response);
    void                redirect(HTTPResponse& response);

    void                getFile(HTTPResponse& response) const;
    void                getDirectory(HTTPResponse& response);
    void                getAutoindex(HTTPResponse& response);
    void                postFile(HTTPResponse& response) const;

    void                cgiQuery(HTTPResponse& response, const std::string& cgi_path) const;
    void                runCgi(const std::string& cgi_path, FILE* cgi_out_file) const;
    void                makeCgiEnv(std::vector<char*>& envp_data) const;
    void                processCgi(pid_t cgi_pid, FILE* cgi_out_file, HTTPResponse& response) const;
    void                makeCgiResponse(HTTPResponse& response, const char* cgi_data, size_t n) const;

    static std::string  getFormFileName(const char* form_header_first, const char* form_header_last);

private:
    const Location&     location_;
    const HTTPRequest&  request_;
    const in_addr_t     server_ip_;
    const in_port_t     server_port_;
    const in_addr&      remote_addr_;

    std::string         pure_uri_;
    std::string         query_string_;
    std::string         path_info_;

    FileInfo            file_info_;

    class HTTPError : public std::exception
    {
    public:
        HTTPError(HTTPStatus status) : status_(status) {}
        HTTPStatus  status() const {return status_;}
    private:
        HTTPStatus  status_;
    };

    typedef void (SimpleHandler::*handler)(HTTPResponse&);
    static const std::pair<std::string, handler>    handlers_init_list_[];
    static const std::map<std::string, handler>     handlers_;
    static const std::map<HTTPStatus, std::string>  http_status_;
};
#endif
