#ifndef AHTTPMESSAGE_HPP
# define AHTTPMESSAGE_HPP

# include <string>
# include <map>

class AHTTPMessage
{
public:
    AHTTPMessage() : _start_line(), _raw_data(), _header() {}

    virtual void                        append(const char* s, size_t n) = 0;
    const std::string                   &start_line() const {return _start_line;}

protected:
    std::string                         _start_line;
    std::string                         _raw_data;
    std::map<std::string, std::string>  _header;
};
#endif
