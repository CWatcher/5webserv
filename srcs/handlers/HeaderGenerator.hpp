
#ifndef HEADERGENERATOR_HPP
# define HEADERGENERATOR_HPP

# include "base/Handler.hpp"

class HeaderGenerator : public Handler
{

public:
    HeaderGenerator(IHandler *next_handler = NULL);
    virtual void handle(const Location &config, const HTTPRequest &input, HTTPMessage &output);

private:
    bool generateHeader(const HTTPRequest &input, HTTPMessage &output) const;

};

#endif
