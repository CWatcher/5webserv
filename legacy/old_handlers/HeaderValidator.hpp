
#ifndef HEADERVALIDATOR_HPP
# define HEADERVALIDATOR_HPP

# include "base/Handler.hpp"

class HeaderValidator : public Handler
{

public:
    HeaderValidator(IHandler *next_handler = NULL);
    virtual void handle(const Location &location, const HTTPRequest &request, HTTPResponse &response);

private:
    bool checkHeader(const HTTPRequest &input) const;

};

#endif
