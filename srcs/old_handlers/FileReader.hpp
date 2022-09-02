
#ifndef FILEREADER_HPP
# define FILEREADER_HPP

# include "base/Handler.hpp"

class FileReader : public Handler
{

public:
    FileReader(IHandler *next_handler = NULL);
    virtual void handle(const Location &location, const HTTPRequest &request, HTTPResponse &response);

private:
    bool readFile(const HTTPRequest &request, HTTPResponse &response) const;

};

#endif
