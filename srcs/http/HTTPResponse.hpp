#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include "http/HTTPMessage.hpp"

# include <iterator>

class HTTPResponse : public HTTPMessage
{
public:
    HTTPResponse() : HTTPMessage() {}

    void                buildResponse(const char* body = NULL, size_t n = 0, const std::string& status_line = "200 OK");
    template <class InputIterator>
    void                buildResponse(InputIterator first, InputIterator last, const std::string& status_line = "200 OK")
    {
        buildHeader(status_line);
        _raw_data.append(first, last);
    }

    const std::string   &raw_data() const {return _raw_data;}

    void                addHeader(const std::string &key, const std::string &value) {_header[key] = value;}
    void                setContentType(const std::string &file_type);
    void                setContentLength(size_t n);

private:
    void                buildHeader(const std::string& status_line );

private:
    static const std::map<std::string, std::string> _mime_type;
};
#endif
