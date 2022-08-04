#include "HTTPResponse.hpp"

#include <iostream>

static const std::pair<std::string, std::string> init_list[] =
{
	std::make_pair("pdf", "application/pdf"),
	std::make_pair("gif", "image/gif"),
	std::make_pair("jpeg", "image/jpeg"),
	std::make_pair("png", "image/png"),
	std::make_pair("tiff", "image/tiff"),
    std::make_pair("bmp", "image/bmp"),
	std::make_pair("css", "text/css"),
	std::make_pair("csv", "text/csv"),
	std::make_pair("html", "text/html"),
	std::make_pair("plain", "text/plain"),
	std::make_pair("xml", "text/xml"),
};

const std::map<std::string, std::string>    HTTPResponse::_types(init_list, init_list + sizeof(init_list) / sizeof(init_list[0]));

void	HTTPResponse::buildResponse(const char* body, size_t n)
{
	_raw_data += "HTTP/1.1 200 OK\n";
	for (std::map<std::string, std::string>::const_iterator it = _header.begin(); it != _header.end(); ++it)
	{
		_raw_data += it->first;
		_raw_data += " : ";
		_raw_data += it->second;
		_raw_data.push_back('\n');
	}
	_raw_data.push_back('\n');
	_raw_data.append(body, n);
}
