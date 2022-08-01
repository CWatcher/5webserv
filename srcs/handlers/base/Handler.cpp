
#include "Handler.hpp"

Handler::Handler(IHandler *next_handler)
    : _next(next_handler) {}

Handler::~Handler() {}

void Handler::handle(const Location &config, const HTTPRequest &input, HTTPMessage &output)
{
    if (_next)
        _next->handle(config, input, output);
}
