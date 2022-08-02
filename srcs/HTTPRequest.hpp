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
    const std::string   *getHeaderValue(const std::string &header_key) const;
    const std::string   getHeaderHostName() const;

    const std::string   &uri() const {return _uri;}
    const std::string   &method() const {return _method;}
    const std::string   &start_line() const {return _start_line;}

private:
    void                                parseHeader();
    void                                parseStartingLine();
    std::map<std::string, std::string>  getHeaderMapFromRaw();
    std::pair<std::string, std::string> getHeaderPairFromLine(const std::string &line);

private:
    std::string     _start_line;
    std::string     _method;
    std::string     _uri;
    size_t          _header_size;
    size_t          _body_size;
};
# endif
