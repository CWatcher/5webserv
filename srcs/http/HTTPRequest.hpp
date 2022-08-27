#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include "HTTPMessage.hpp"
# include "config/VirtualServer.hpp"

class HTTPRequest : public HTTPMessage
{
public:
    HTTPRequest() : _start_line(), _method(), _uri(), _header_size(0), _body_size(0) {}
    virtual void        addData(const char* data, size_t n);

    bool                hasEndOfMessage();
    std::string         getHeaderValue(const std::string &header_key) const;
    const std::string   getHeaderHostName() const;

    const std::string   &start_line() const {return _start_line;}
    const std::string   &method() const {return _method;}
    const std::string   &uri() const {return _uri;}
    const std::string   &http() const {return _http;}
    const char*         body() const {return _raw_data.data() + _header_size;}
    size_t              body_size() const {return _body_size;}

private:
    void                                parseHeader();
    void                                parseStartLine();
    std::map<std::string, std::string>  getHeaderMapFromRaw();
    std::pair<std::string, std::string> getHeaderPairFromLine(const std::string &line);

private:
    std::string     _start_line;
    std::string     _method;
    std::string     _uri;
    std::string     _http;
    size_t          _header_size;
    size_t          _body_size;
};
# endif
