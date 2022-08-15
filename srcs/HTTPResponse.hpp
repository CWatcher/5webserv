#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include "HTTPMessage.hpp"
# include "HTTPStatus.hpp"

class HTTPResponse : public HTTPMessage
{
public:
    HTTPResponse() : HTTPMessage() {}

    virtual void        buildResponse(const char* body, size_t n, HTTPStatus status = OK);
    void                addHeader(const std::string &key, const std::string &value) {_header[key] = value;}

    const std::string   &raw_data() const {return _raw_data;}
    void                setContentType(const std::string &file_type);
    void                setContentLength(size_t n);

private:
    static const std::map<std::string, std::string> _mime_type;
    static const std::map<HTTPStatus, std::string>  _http_status;
};
#endif
