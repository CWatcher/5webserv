#include "HTTPResponse.hpp"

#include <iostream>

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
