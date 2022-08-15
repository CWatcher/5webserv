#ifndef SIMPLEHANDLER_HPP
# define SIMPLEHANDLER_HPP

# include "HTTPResponse.hpp"
# include "HTTPRequest.hpp"
# include "HTTPStatus.hpp"
# include "config/ServerConfig.hpp"

# include <exception>

class SimpleHandler
{
public:
    SimpleHandler(const Location& loc, const HTTPRequest& req, HTTPResponse& resp) : location_(loc), request_(req), response_(resp) {}

    void    makeResponse();

private:
    void    get();
    void    post();
    void    del();

    void    validateRequest();
    void    error(HTTPStatus);
    void    redirect(unsigned);

private:
    const Location&     location_;
    const HTTPRequest&  request_;
    HTTPResponse        response_;

    std::string         path_;
    //query stritn, path translated, ...
    //cgi
    //file name, type ??

    class HTTPError : public std::exception
    {
    public:
        HTTPError(HTTPStatus status) : status_(status) {}
        HTTPStatus  status() const {return status_;}
    private:
        HTTPStatus  status_;
    };

    typedef void (SimpleHandler::*handler)();
    static const std::pair<std::string, handler>    handlers_init_list_[];
    static const std::map<std::string, handler>     handlers_;
};
#endif
