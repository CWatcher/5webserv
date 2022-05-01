
#include "HeaderValidator.hpp"

HeaderValidator::HeaderValidator(IHandler *next_handler)
    : Handler(next_handler) {}

void HeaderValidator::handle(HTTPMessage &input, HTTPMessage &output)
{
    _success = checkHeader(input);

    Handler::handle(input, output);
}

bool HeaderValidator::checkHeader(const HTTPMessage &input) const
{
    (void)input;
    // do all checks

    return true;
}