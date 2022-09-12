
#include "HeaderParser.hpp"

HeaderParser::HeaderParser(IHandler *next_handler)
    : Handler(next_handler) {}

void HeaderParser::handle(const Location &location, const HTTPRequest &request, HTTPResponse &response)
{
    if (parseHeader(request))
        Handler::handle(location, request, response);
}

bool HeaderParser::parseHeader(const HTTPRequest &input) const
{
    (void)input;
    // do all work

    return true;
}
