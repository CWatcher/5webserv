
#include "HTTPMessage.hpp"

#include <cstdlib>
#include <cerrno>

HTTPMessage::HTTPMessage()
	: raw_data()
	, _header_size(0)
	, _body_size(0) {}

HTTPMessage::HTTPMessage(const HTTPMessage &src)
	: raw_data(src.raw_data)
	, _header_size(src._header_size)
	, _body_size(src._body_size) {}

HTTPMessage::~HTTPMessage() {}

HTTPMessage	&HTTPMessage::operator=(const HTTPMessage &rhs)
{
	if (this != &rhs)
	{
		this->raw_data = rhs.raw_data;
		this->_header_size = rhs._header_size;
	}
	return *this;
}
void		HTTPMessage::operator+=(const std::string &rhs)
{
	raw_data += rhs;

	if (header.empty())
		getHeader();

	if (!header.empty())
		_body_size = raw_data.size() - _header_size;
}

bool	HTTPMessage::hasEndOfMessage()
{
	const bool	hasHeader = !header.empty();
	std::string	*content_length_str;
	long int	content_length;
	bool		end_found;

	if (raw_data.compare(0, 3, "GET") || raw_data.compare(0, 6, "DELETE"))
		end_found = hasHeader;
	else if (hasHeader && (raw_data.compare(0, 4, "POST") || raw_data.compare(0, 3, "PUT")))
	{
		content_length_str = getHeaderValue("Content-Length");
		if (content_length_str == NULL)
			end_found = true;
		else
		{
			content_length = std::strtol(content_length_str->c_str(), NULL, 10);
			if (errno || content_length < 0)
			{
				logger::cerrno();
				end_found = true;
			}
			else
				end_found = (_body_size >= static_cast<size_t>(content_length));
		}
	}
	else // method unknown, validator will deal with it
		end_found = true;
	return end_found;
}

std::string	*HTTPMessage::getHeaderValue(const std::string &header_key)
{
	std::map<std::string, std::string>::iterator found;

	found = header.find(strLowerCaseCopy(header_key));
	if (found != header.end())
		return &found->second;
	else
		return NULL;
}

void	HTTPMessage::getHeader()
{
	const size_t	header_end = raw_data.find("\r\n\r\n");

	if (header_end != std::string::npos)
	{
		_header_size = header_end + 4;
		header = getHeaderMapFromRaw();
		logger::debug("HTTP request header found");
	}
	else
		logger::debug("HTTP request header not found yet");
}

std::map<std::string, std::string>	HTTPMessage::getHeaderMapFromRaw()
{
	std::map<std::string, std::string>	header_map;
	size_t		line_start;
	size_t		line_end;
	std::string line;

	line_end = raw_data.find('\n', 0);
	while (true)
	{
		line_start = line_end + (raw_data[line_end] == '\r') + 1;
		line_end = raw_data.find('\n', line_start);
		if (raw_data[line_end - 1] == '\r')
			--line_end;
		if (line_start == line_end)
			break ;

		line = raw_data.substr(line_start, line_end - line_start);
		header_map.insert(getHeaderPairFromLine(line));
	}
	return header_map;
}

std::pair<std::string, std::string>	HTTPMessage::getHeaderPairFromLine(const std::string &line)
{
	const size_t	key_end = line.find(':');
	std::string		key;
	std::string		value;

	if (key_end == std::string::npos)
	{
		key = line;
		value = "";
	}
	else
	{
		key = line.substr(0, key_end);
		value = line.substr(key_end + 1, std::string::npos);
	}
	strTrim(strLowerCase(key));
	strTrim(value);

	return std::make_pair(key, value);
}
