
#include "CGIReader.hpp"

CGIReader::CGIReader(IHandler *next_handler)
    : Handler(next_handler) {}

void CGIReader::handle(const Location &location, const HTTPRequest &request, HTTPResponse &response)
{
    if (runScript(request, response))
        Handler::handle(location, request, response);
}

bool CGIReader::runScript(const HTTPRequest &request, HTTPResponse &response) const
{
    (void)request;
    (void)response;
    // do all work

    return true;
}
