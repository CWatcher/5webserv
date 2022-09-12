
#ifndef HEADERPARSER_HPP
# define HEADERPARSER_HPP

# include "base/Handler.hpp"

class HeaderParser : public Handler
{

public:
    HeaderParser(IHandler *next_handler = NULL);
    virtual void handle(const Location &location, const HTTPRequest &request, HTTPResponse &response);

private:
    bool parseHeader(const HTTPRequest &input) const;

};

#endif