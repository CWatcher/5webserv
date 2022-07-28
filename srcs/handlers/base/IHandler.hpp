
#ifndef IHANDLER_HPP
# define IHANDLER_HPP

# include "HTTPMessage.hpp"
# include "config/ServerConfig.hpp"

class IHandler
{
public:
    virtual ~IHandler() {};

    virtual void handle(const Location &config, const HTTPMessage &input, HTTPMessage &output) = 0;
};


#endif
