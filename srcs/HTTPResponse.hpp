#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include "HTTPMessage.hpp"
# include "HTTPStatus.hpp"

class HTTPResponse : public HTTPMessage
{
public:
    HTTPResponse() : HTTPMessage() {}

    virtual void        buildResponse(const char* body = NULL, size_t n = 0, HTTPStatus status = OK);
    void                addHeader(const std::string &key, const std::string &value) {_header[key] = value;}

    const std::string   &raw_data() const {return _raw_data;}
    void                setContentType(const std::string &file_type);
    void                setContentLength(size_t n);

    static const std::map<HTTPStatus, std::string>  http_status_msg;
private:
    static const std::map<std::string, std::string> _mime_type;
};
#endif
