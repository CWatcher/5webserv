
#include "HeaderParser.hpp"

HeaderParser::HeaderParser(IHandler *next_handler)
    : Handler(next_handler) {}

void HeaderParser::handle(const Location &config, const HTTPRequest &input, HTTPMessage &output)
{
    if (parseHeader(input))
        Handler::handle(config, input, output);
}

bool HeaderParser::parseHeader(const HTTPRequest &input) const
{
    (void)input;
    // do all work

    return true;
}
