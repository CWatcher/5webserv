
#include "HeaderGenerator.hpp"

HeaderGenerator::HeaderGenerator(IHandler *next_handler)
    : Handler(next_handler) {}

void HeaderGenerator::handle(const Location &config, const HTTPRequest &input, HTTPResponse &output)
{
    if (generateHeader(input, output))
        Handler::handle(config, input, output);
}

bool HeaderGenerator::generateHeader(const HTTPRequest &input, HTTPResponse &output) const
{
    (void)input;
    (void)output;
    // do all work
    output.addHeader("Date", "Wed, 18 Feb 2021 11:20:59 GMT");
    output.addHeader("Server", "Apache");
    output.addHeader("X-Powered-By", "webserv");
    output.addHeader("Last-Modified", "Wed, 11 Feb 2009 11\", 2\"0\", 5\"9 GMT");
    output.addHeader("Content-Type", "text/html; charset=utf-8");
    output.addHeader("Content-Length", "14");
    output.addHeader("Connection", "close");

    output.buildResponse("<h1>HELLO WOR!D</h1>\n", 21);

    return true;
}
