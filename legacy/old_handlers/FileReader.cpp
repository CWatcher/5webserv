
#include "FileReader.hpp"

FileReader::FileReader(IHandler *next_handler)
    : Handler(next_handler) {}

void FileReader::handle(const Location &location, const HTTPRequest &request, HTTPResponse &response)
{
    if (readFile(request, response))
        Handler::handle(location, request, response);
}

bool FileReader::readFile(const HTTPRequest &request, HTTPResponse &response) const
{
    (void)request;
    (void)response;
    // do all work

    return true;
}
