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
    SimpleHandler(const Location& loc, const HTTPRequest& req);

    void                fillResponse(HTTPResponse& response);

private:
    void                get(HTTPResponse& response);
    void                post(HTTPResponse& response);
    void                del(HTTPResponse& response);

    void                error(HTTPStatus status, HTTPResponse& response);
    void                redirect(HTTPResponse& response);

    void                getFile(HTTPResponse& response);
    void                getDirectory(HTTPResponse& response);
    void                getAutoindex(HTTPResponse& response);
    void                postFile(HTTPResponse& response);
    void                cgiHandler(HTTPResponse& response) const;

    static std::string  getFormFileName(const char* form_header_first, const char* form_header_last);

private:
    const Location&     location_;
    const HTTPRequest&  request_;

    FileInfo            file_info_;
    std::string         pure_uri_;
    std::string         query_string_;
    std::string         path_info_;

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
