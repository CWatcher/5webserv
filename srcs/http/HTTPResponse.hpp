#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include "http/HTTPMessage.hpp"

# include <iterator>

class HTTPResponse : public HTTPMessage
{
public:
    HTTPResponse();

    void                addHeader(const std::string &key, const std::string &value) {_header[key] = value;}
    void                setContentType(const std::string &file_type);
    void                setContentLength(size_t n);

    template <class InputIterator>
    void                buildResponse(InputIterator first, InputIterator last, bool body = true, const std::string& status_line = "200 OK")
    {
        buildHeader(status_line);
        if (body)
            _raw_data.append(first, last);
    }

private:
    void                buildHeader(const std::string& status_line);

private:
    static const std::map<std::string, std::string> _mime_type;
};
#endif
