
#ifndef HEADERGENERATOR_HPP
# define HEADERGENERATOR_HPP

# include "base/Handler.hpp"

class HeaderGenerator : public Handler
{

public:
    HeaderGenerator(IHandler *next_handler = NULL);
    virtual void handle(const BaseConfig &config, const HTTPMessage &input, HTTPMessage &output);

private:
    bool generateHeader(const HTTPMessage &input, HTTPMessage &output) const;

};

#endif
