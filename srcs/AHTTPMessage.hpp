#ifndef AHTTPMESSAGE_HPP
# define AHTTPMESSAGE_HPP

# include <string>
# include <map>

class AHTTPMessage
{
public:
    AHTTPMessage() : _raw_data(), _header() {}

protected:
    std::string                         _raw_data;
    std::map<std::string, std::string>  _header;
};
#endif
