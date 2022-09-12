
#ifndef HANDLER_HPP
# define HANDLER_HPP

# include "IHandler.hpp"
# include <cstdlib>

class Handler : public IHandler
{
public:
    virtual	~Handler();

    virtual void handle(const Location &location, const HTTPRequest &request, HTTPResponse &response);

protected:
    Handler(IHandler *next_handler);

private:
    IHandler *_next;

};

#endif
