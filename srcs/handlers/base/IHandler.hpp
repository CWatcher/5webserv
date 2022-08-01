
#ifndef IHANDLER_HPP
# define IHANDLER_HPP

# include "HTTPRequest.hpp"
# include "HTTPResponse.hpp"
# include "config/ServerConfig.hpp"

class IHandler
{
public:
    virtual ~IHandler() {};

    virtual void handle(const Location &config, const HTTPRequest &input, HTTPResponse &output) = 0;
};


#endif
