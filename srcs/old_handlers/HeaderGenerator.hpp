
#ifndef HEADERGENERATOR_HPP
# define HEADERGENERATOR_HPP

# include "base/Handler.hpp"

class HeaderGenerator : public Handler
{

public:
    HeaderGenerator(IHandler *next_handler = NULL);
    virtual void handle(const Location &location, const HTTPRequest &request, HTTPResponse &response);

private:
    bool generateHeader(const HTTPRequest &request, HTTPResponse &response) const;

};

#endif
