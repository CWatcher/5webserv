
#include "FileReader.hpp"

FileReader::FileReader(IHandler *next_handler)
    : Handler(next_handler) {}

void FileReader::handle(HTTPMessage &input, HTTPMessage &output)
{
    _success = readFile(input, output);

    Handler::handle(input, output);
}

bool FileReader::readFile(const HTTPMessage &input, HTTPMessage &output) const
{
    (void)input;
    (void)output;
    // do all work

    return true;
}