
#include "HeaderGenerator.hpp"

HeaderGenerator::HeaderGenerator(IHandler *next_handler)
    : Handler(next_handler) {}

void HeaderGenerator::handle(HTTPMessage &input, HTTPMessage &output)
{
    _success = generateHeader(input, output);

    Handler::handle(input, output);
}

bool HeaderGenerator::generateHeader(const HTTPMessage &input, HTTPMessage &output) const
{
    (void)input;
    (void)output;
    // do all work
    const char      *DefaultResponse =	  "HTTP/1.1 200 OK\n"
                                            "Date: Wed, 18 Feb 2021 11:20:59 GMT\n"
                                            "Server: Apache\n"
                                            "X-Powered-By: webserv\n"
                                            "Last-Modified: Wed, 11 Feb 2009 11:20:59 GMT\n"
                                            "Content-Type: text/html; charset=utf-8\n"
                                            "Content-Length: 14\n"
                                            "Connection: close\n"
                                            "\n"
                                            "<h1>HELLO WOR!D</h1>\n";
    output.raw_data = DefaultResponse;

    return true;
}