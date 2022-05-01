
#ifndef HANDLER_HPP
# define HANDLER_HPP

# include "IHandler.hpp"
# include <cstdlib>

class Handler : public IHandler
{
public:
    virtual	~Handler();

    virtual void handle(HTTPMessage &input, HTTPMessage &output);

protected:
    Handler(IHandler *next_handler);


protected:
    bool     _success;

private:
    IHandler *_next;

};

#endif
