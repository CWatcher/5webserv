
#include "HeaderValidator.hpp"

HeaderValidator::HeaderValidator(IHandler *next_handler)
    : Handler(next_handler) {}

void HeaderValidator::handle(const Location &location, const HTTPRequest &request, HTTPResponse &response)
{
    if (checkHeader(request))
        Handler::handle(location, request, response);
}

bool HeaderValidator::checkHeader(const HTTPRequest &input) const
{
    (void)input;
    // do all checks

    return true;
}
