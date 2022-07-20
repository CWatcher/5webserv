
#ifndef HEADERVALIDATOR_HPP
# define HEADERVALIDATOR_HPP

# include "base/Handler.hpp"

class HeaderValidator : public Handler
{

public:
    HeaderValidator(IHandler *next_handler = NULL);
    virtual void handle(const BaseConfig &config, const HTTPMessage &input, HTTPMessage &output);

private:
    bool checkHeader(const HTTPMessage &input) const;

};

#endif
