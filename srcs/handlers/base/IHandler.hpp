
#ifndef IHANDLER_HPP
# define IHANDLER_HPP

#include "HTTPMessage.hpp"

class IHandler
{
public:
    virtual ~IHandler() {};

    virtual void handle(HTTPMessage &input, HTTPMessage &output) = 0;
};


#endif
