#ifndef AHANDLER_HPP
# define AHANDLER_HPP

# include "http/HTTPResponse.hpp"
# include "http/HTTPRequest.hpp"
# include "http/HTTPStatus.hpp"
# include "config/ServerConfig.hpp"

class AHandler
{
public:
    AHandler(const Location& loc, const HTTPRequest& req);
    virtual ~AHandler() {}

    void                makeResponse(HTTPResponse& response);

protected:
    void                error(HTTPStatus::_ status, HTTPResponse& response);
    void                redirect(HTTPResponse& response);

private:
    virtual void        handle(HTTPResponse& response) = 0;
    static void         normalizeUri(std::string& uri);

protected:
    const Location&     location_;
    const HTTPRequest&  request_;

    std::string         pure_uri_;
    std::string         query_string_;
    std::string         path_info_;

    class HTTPError : public std::exception
    {
    public:
        HTTPError(HTTPStatus::_ status) : status_(status) {}
        HTTPStatus::_  status() const {return status_;}
    private:
        HTTPStatus::_  status_;
    };

    static const std::map<HTTPStatus::_, std::string>  http_status_;
};

#endif
