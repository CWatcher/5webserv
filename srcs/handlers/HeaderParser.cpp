
#include "HeaderParser.hpp"

HeaderParser::HeaderParser(IHandler *next_handler)
    : Handler(next_handler) {}

void HeaderParser::handle(const BaseConfig &config, const HTTPMessage &input, HTTPMessage &output)
{
    if (parseHeader(input))
        Handler::handle(config, input, output);
}

bool HeaderParser::parseHeader(const HTTPMessage &input) const
{
    (void)input;
    // do all work

    return true;
}
