
#include "HeaderValidator.hpp"

HeaderValidator::HeaderValidator(IHandler *next_handler)
    : Handler(next_handler) {}

void HeaderValidator::handle(const Location &config, const HTTPMessage &input, HTTPMessage &output)
{
    if (checkHeader(input))
        Handler::handle(config, input, output);
}

bool HeaderValidator::checkHeader(const HTTPMessage &input) const
{
    (void)input;
    // do all checks

    return true;
}
