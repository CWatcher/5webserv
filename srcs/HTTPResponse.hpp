#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include "HTTPMessage.hpp"
# include "HTTPStatus.hpp"

# include <iterator>

class HTTPResponse : public HTTPMessage
{
public:
    HTTPResponse() : HTTPMessage() {}

    void                buildResponse(const char* body = NULL, size_t n = 0, HTTPStatus status = OK);
    void                buildResponse(std::string& body, HTTPStatus status = OK);
    template <class InputIterator>
    void                buildResponse(InputIterator first, InputIterator last, HTTPStatus status = OK)
    {
        buildHeader(status);
        _raw_data.append(first, last);
    }

    const std::string   &raw_data() const {return _raw_data;}

    void                addHeader(const std::string &key, const std::string &value) {_header[key] = value;}
    void                setContentType(const std::string &file_type);
    void                setContentLength(size_t n);

    static const std::map<HTTPStatus, std::string>  http_status_msg;
private:
    void                buildHeader(HTTPStatus status);
private:
    static const std::map<std::string, std::string> _mime_type;
};
#endif
