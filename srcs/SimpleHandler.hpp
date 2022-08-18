#ifndef SIMPLEHANDLER_HPP
# define SIMPLEHANDLER_HPP

# include "http/HTTPResponse.hpp"
# include "http/HTTPRequest.hpp"
# include "http/HTTPStatus.hpp"
# include "config/ServerConfig.hpp"

# include <exception>

class SimpleHandler
{
public:
    SimpleHandler(const Location& loc, const HTTPRequest& req) : location_(loc), request_(req) {}

    void    fillResponse(HTTPResponse&  response);

private:
    void    get(HTTPResponse&  response);
    void    post(HTTPResponse&  response);
    void    del(HTTPResponse&  response);

    void    validateRequest();
    void    error(HTTPStatus status, HTTPResponse& response);
    void    redirect(unsigned status, HTTPResponse& response);

    void    getFile(HTTPResponse& response);
    void    getAutoindex(HTTPResponse& response);

private:
    const Location&     location_;
    const HTTPRequest&  request_;

    std::string         path_;
    std::string         file_name_;
    std::string         type_;
    //query stritn, path translated, cgi ...

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
};
#endif
