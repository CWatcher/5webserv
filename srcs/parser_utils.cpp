#include "parser_utils.hpp"

#include <exception>
#include <sstream>

void	parse_single_str_value(std::ifstream& f, std::string& value)
{
	std::string			line;
	std::stringstream	ss;

	std::getline(f >> std::ws, line, ';');
	ss.str(line);
	ss >> value;
	if (value.empty())
		throw std::logic_error("empty value");
	ss >> std::ws;
	if (ss.peek() != std::char_traits<char>::eof())
		throw std::logic_error("too many values");
}

void	parse_uint_value(std::ifstream& f, unsigned& value)
{
	std::string			line;
	std::stringstream	ss;

	std::getline(f >> std::ws, line, ';');
	if (line.empty())
		throw std::logic_error("empty value");
	if (line[0] == '-' || line[0] == '+')
		throw std::logic_error("bad integer value");
	ss.str(line);
	ss >> value;
	if (ss.fail() || (ss.peek() != std::char_traits<char>::eof() && !std::isspace(ss.peek())))
		throw std::logic_error("bad integer value");
	ss >> std::ws;
	if (ss.peek() != std::char_traits<char>::eof())
		throw std::logic_error("too many values");
}

void	parse_autoindex(std::ifstream& f, bool& value)
{
	std::string			line;
	std::stringstream	ss;

	std::getline(f >> std::ws, line, ';');
	if (line.empty())
		throw std::logic_error("autoindex empty value");
	ss.str(line);
	ss >> line;
	if (line == "on")
		value = true;
	else if (line == "off")
		value = false;
	else
		throw std::logic_error("autoindex bad value");
	ss >> std::ws;
	if (ss.peek() != std::char_traits<char>::eof())
		throw std::logic_error("autoindex too many values");
}

void	parse_index(std::ifstream& f, std::vector<std::string>& value)
{
	std::string			str;
	std::stringstream	ss;

	std::getline(f, str, ';');
	ss.str(str);
	while (ss >> str)
		value.push_back(str);
}
