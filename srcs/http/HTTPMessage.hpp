#ifndef HTTPMESSAGE_HPP
# define HTTPMESSAGE_HPP

# include <string>
# include <map>

class HTTPMessage
{
public:
    HTTPMessage() : _raw_data(), _header() {}

    const std::string                           &raw_data() const {return _raw_data;}
    const std::map<std::string, std::string>    &header() const {return _header;}
protected:
    std::string                                 _raw_data;
    std::map<std::string, std::string>          _header;
};
#endif
