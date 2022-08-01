#ifndef HTTPREQUEST
# define HTTPREQUEST

# include "AHTTPMessage.hpp"
# include "config/VirtualServer.hpp"

class HTTPRequest : public AHTTPMessage
{
public:
    HTTPRequest() : _start_line(), _method(), _uri(), _header_size(0), _body_size(0), _location(NULL) {}

    virtual void        addData(const char* data, size_t n);

    bool                hasEndOfMessage();
    const std::string   *getHeaderValue(const std::string &header_key) const;
    const std::string   getHeaderHostName() const;

    const std::string   &uri() {return _uri;}
    const std::string   &method() {return _method;}
    const std::string   &start_line() const {return _start_line;}
    const Location      *location() {return _location;}
    void                setLocation(const Location &location) {_location = &location;}

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
    const Location  *_location;
};
# endif
