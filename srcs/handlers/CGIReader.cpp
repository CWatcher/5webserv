
#include "CGIReader.hpp"

CGIReader::CGIReader(IHandler *next_handler)
    : Handler(next_handler) {}

void CGIReader::handle(HTTPMessage &input, HTTPMessage &output)
{
    if (runScript(input, output))
        Handler::handle(input, output);
}

bool CGIReader::runScript(const HTTPMessage &input, HTTPMessage &output) const
{
    (void)input;
    (void)output;
    // do all work

    return true;
}