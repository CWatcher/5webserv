#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include "HTTPMessage.hpp"

class HTTPResponse : public HTTPMessage
{
public:
    HTTPResponse() : _status(200) {}

    virtual void        buildResponse(const char* body, size_t n);
    void                addHeader(const std::string &key, const std::string &value) {_header[key] = value;}

    int                 status() const {return _status;}
    void                setStatus(int status) {_status = status;}
    const std::string   &raw_data() const {return _raw_data;}
    void                setContentType();

private:
    int                                             _status; //??
    static const std::map<std::string, std::string> _types;
};
#endif
