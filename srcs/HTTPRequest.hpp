#ifndef HTTPREQUEST
# define HTTPREQUEST

# include "AHTTPMessage.hpp"

class HTTPRequest : public AHTTPMessage
{
public:
    HTTPRequest() : _method(), _uri(), _header_size(0), _body_size(0) {}

    virtual void        append(const char* s, size_t n);

    bool                hasEndOfMessage();
    const std::string   *getHeaderValue(const std::string &header_key) const;
    const std::string   getHeaderHostName() const;

    const std::string   &uri() {return _uri;}
    const std::string   &method() {return _method;}

private:
    void                                parseHeader();
    void                                parseStartingLine();
    std::map<std::string, std::string>  getHeaderMapFromRaw();
    std::pair<std::string, std::string> getHeaderPairFromLine(const std::string &line);

private:
    std::string _method;
    std::string _uri;
    size_t      _header_size;
    size_t      _body_size;
};

# endif
