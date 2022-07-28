
#ifndef HEADERPARSER_HPP
# define HEADERPARSER_HPP

# include "base/Handler.hpp"

class HeaderParser : public Handler
{

public:
    HeaderParser(IHandler *next_handler = NULL);
    virtual void handle(const Location &config, const HTTPMessage &input, HTTPMessage &output);

private:
    bool parseHeader(const HTTPMessage &input) const;

};

#endif
