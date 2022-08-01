
#ifndef HEADERVALIDATOR_HPP
# define HEADERVALIDATOR_HPP

# include "base/Handler.hpp"

class HeaderValidator : public Handler
{

public:
    HeaderValidator(IHandler *next_handler = NULL);
    virtual void handle(const Location &config, const HTTPRequest &input, HTTPResponse &output);

private:
    bool checkHeader(const HTTPRequest &input) const;

};

#endif
