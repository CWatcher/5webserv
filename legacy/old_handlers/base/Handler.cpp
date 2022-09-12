
#include "Handler.hpp"

Handler::Handler(IHandler *next_handler)
    : _next(next_handler) {}

Handler::~Handler() {}

void Handler::handle(const Location &config, const HTTPRequest &request, HTTPResponse &response)
{
    if (_next)
        _next->handle(config, request, response);
}
