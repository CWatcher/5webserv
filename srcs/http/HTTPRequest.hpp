#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include "HTTPMessage.hpp"
# include "config/VirtualServer.hpp"

# include <stdint.h> // SIZE_MAX

class HTTPRequest : public HTTPMessage
{
	enum ChunkPart {chunk_size, chunk_data};
public:
    // HTTPRequest() : _method(), _uri(), _header_size(0), _body_size(0), _chunk_size(SIZE_MAX), _chunk_part(chunk_size) {}
    HTTPRequest() : _method(), _uri(), _header_size(0), _body_size(0), _chunk_size(SIZE_MAX) {}

    void                addData(const char* data, size_t n);

    bool                isRequestReceived();
    std::string         getHeaderValue(const std::string &header_key) const;
    std::string         getHeaderHostName() const;
    std::string         getHeaderParameter(const std::string& key, const std::string& param) const;

    const std::string   &method() const {return _method;}
    const std::string   &uri() const {return _uri;}
    const std::string   &http() const {return _http;}
    const char*         body() const {return _raw_data.data() + _header_size;}
    size_t              body_size() const {return _body_size;}
    size_t              body_offset() const {return _header_size;}

    bool                isFormData() const {return getHeaderValue("Content-Type").find("multipart/form-data") != std::string::npos;}

private:
    void                fillHeaderMap();
    void                parseStartLine();
    void                parseHeader(size_t header_end);
    void                parseHeaderLine(const std::string &line);
	bool				unchunk();

private:
	static			const std::string terminator;// = "\r\n";
    std::string     _method;
    std::string     _uri;
    std::string     _http;
    size_t          _header_size;
    size_t          _body_size;
	size_t			_chunk_size;
	// ChunkPart		_chunk_part;
	size_t			_chunk_pos;
	// std::string::iterator _iterator;
};

# endif
