
#include "HeaderGenerator.hpp"

HeaderGenerator::HeaderGenerator(IHandler *next_handler)
    : Handler(next_handler) {}

void HeaderGenerator::handle(const Location &location, const HTTPRequest &request, HTTPResponse &response)
{
    if (generateHeader(request, response))
        Handler::handle(location, request, response);
}

bool HeaderGenerator::generateHeader(const HTTPRequest &request, HTTPResponse &response) const
{
    (void)request;
    (void)response;
    // do all work
    response.addHeader("Date", "Wed, 18 Feb 2021 11:20:59 GMT");
    response.addHeader("Server", "Apache");
    response.addHeader("X-Powered-By", "webserv");
    response.addHeader("Last-Modified", "Wed, 11 Feb 2009 11\", 2\"0\", 5\"9 GMT");
    response.addHeader("Content-Type", "text/html; charset=utf-8");
    response.addHeader("Content-Length", "14");
    response.addHeader("Connection", "close");

    response.buildResponse("<h1>HELLO WOR!D</h1>\n", 21);

    return true;
}
