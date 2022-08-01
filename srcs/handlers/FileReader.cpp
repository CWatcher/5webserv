
#include "FileReader.hpp"

FileReader::FileReader(IHandler *next_handler)
    : Handler(next_handler) {}

void FileReader::handle(const Location &config, const HTTPRequest &input, HTTPResponse &output)
{
    if (readFile(input, output))
        Handler::handle(config, input, output);
}

bool FileReader::readFile(const HTTPRequest &input, HTTPResponse &output) const
{
    (void)input;
    (void)output;
    // do all work

    return true;
}
