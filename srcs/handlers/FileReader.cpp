
#include "FileReader.hpp"

FileReader::FileReader(IHandler *next_handler)
    : Handler(next_handler) {}

void FileReader::handle(const Location &config, const HTTPMessage &input, HTTPMessage &output)
{
    if (readFile(input, output))
        Handler::handle(config, input, output);
}

bool FileReader::readFile(const HTTPMessage &input, HTTPMessage &output) const
{
    (void)input;
    (void)output;
    // do all work

    return true;
}
