#ifndef HTTPMESSAGE_HPP
# define HTTPMESSAGE_HPP

# include <string>
# include <map>

# define BUFER_SIZE 8192UL

class HTTPMessage
{
public:
    HTTPMessage() : _buffer(), _header() {}

    const std::string                           &buffer() const {return _buffer;}
    const std::map<std::string, std::string>    &header() const {return _header;}
protected:
    std::string                                 _buffer;
    std::map<std::string, std::string>          _header;
};
#endif
