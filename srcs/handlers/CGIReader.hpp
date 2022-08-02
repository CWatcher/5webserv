
#ifndef CGIREADER_HPP
# define CGIREADER_HPP

# include "base/Handler.hpp"

class CGIReader : public Handler
{

public:
    CGIReader(IHandler *next_handler = NULL);
    virtual void handle(const Location &location, const HTTPRequest &request, HTTPResponse &response);

private:
    bool runScript(const HTTPRequest &request, HTTPResponse &response) const;

};

#endif
