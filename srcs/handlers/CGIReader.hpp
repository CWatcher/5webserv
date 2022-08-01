
#ifndef CGIREADER_HPP
# define CGIREADER_HPP

# include "base/Handler.hpp"

class CGIReader : public Handler
{

public:
    CGIReader(IHandler *next_handler = NULL);
    virtual void handle(const Location &config, const HTTPRequest &input, HTTPResponse &output);

private:
    bool runScript(const HTTPRequest &input, HTTPResponse &output) const;

};

#endif
