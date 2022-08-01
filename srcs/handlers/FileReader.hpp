
#ifndef FILEREADER_HPP
# define FILEREADER_HPP

# include "base/Handler.hpp"

class FileReader : public Handler
{

public:
    FileReader(IHandler *next_handler = NULL);
    virtual void handle(const Location &config, const HTTPRequest &input, HTTPResponse &output);

private:
    bool readFile(const HTTPRequest &input, HTTPResponse &output) const;

};

#endif
