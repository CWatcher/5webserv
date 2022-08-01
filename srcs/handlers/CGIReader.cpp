
#include "CGIReader.hpp"

CGIReader::CGIReader(IHandler *next_handler)
    : Handler(next_handler) {}

void CGIReader::handle(const Location &config, const HTTPRequest &input, HTTPMessage &output)
{
    if (runScript(input, output))
        Handler::handle(config, input, output);
}

bool CGIReader::runScript(const HTTPRequest &input, HTTPMessage &output) const
{
    (void)input;
    (void)output;
    // do all work

    return true;
}
