
#ifndef IHANDLER_HPP
# define IHANDLER_HPP

# include "http/HTTPRequest.hpp"
# include "http/HTTPResponse.hpp"
# include "config/ServerConfig.hpp"

class IHandler
{
public:
    virtual ~IHandler() {};

    virtual void handle(const Location &location, const HTTPRequest &request, HTTPResponse &response) = 0;
};


#endif
