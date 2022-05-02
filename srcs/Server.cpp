#include "Server.hpp"
#include "parser_utils.hpp"


#include "utils/log.hpp"//
#include <exception>//
#include <sstream>//
#include <cstdlib>

const std::pair<std::string, Server::f>	Server::parser_init_list_[] = {
	std::make_pair("listen", &Server::parseListen),
	std::make_pair("server_name", &Server::parseServerName),
	std::make_pair("root", &Server::parseRoot),
	std::make_pair("index", &Server::parseIndex),
	std::make_pair("error_page", &Server::parseErrorPage),
	std::make_pair("body_size", &Server::parseBodySize)
};

const std::map<std::string, Server::f>	Server::parser(parser_init_list_, parser_init_list_ + 6);

Server::Server() :
    host_("0.0.0.0"),
    port_("8080"),
    error_page_(), //??
    body_size_(0)
{
	host_port_.sin_addr.s_addr = inet_addr("0.0.0.0");
	host_port_.sin_port = htons(8080);
}

void	Server::parseConfig(std::ifstream& file)
{
	std::string											token;
	std::map<std::string, Server::f>::const_iterator	it;

	file >> token;
	if (token != "{")
		throw std::logic_error("server block error");
	while (true)
	{
		file >> std::ws;
		if (static_cast<char>(file.peek()) == '#')
		{
			std::getline(file, token);
			continue;
		}
		file >> token;
		if (token == "}")
			return;
		it = parser.find(token);
		if (it == parser.end())
			throw std::logic_error( "unknown server directive");
		else
			(this->*it->second)(file);
	}
}

void Server::parseListen(std::ifstream &f)
{
	std::string			line;
	std::stringstream	ss;

	std::getline(f >> std::ws, line, ';');
	if (line.empty())
		throw std::logic_error("listen empty host:port");
	ss.str(line);

	host_.clear();
	std::getline(ss, host_, ':');
	if (host_.empty())
		throw std::logic_error("listen empty host");
	if (host_.find_first_not_of("0123456789.") != std::string::npos)
		throw std::logic_error("listen bad host");
	host_port_.sin_addr.s_addr = inet_addr(host_.c_str());
	if (host_port_.sin_addr.s_addr == INADDR_NONE)
		throw std::logic_error("listen bad host");

	port_.clear();
	std::getline(ss, port_);
	size_t	size = port_.find_last_not_of(" \f\n\r\t\v") + 1;
	if (size != std::string::npos)
		port_.resize(size);
	if (port_.empty())
		throw std::logic_error("listen empty port");
	if (port_.find_first_not_of("0123456789") != std::string::npos)
		throw std::logic_error("listen bad port");
	host_port_.sin_port = htons(strtoul(port_.c_str(), NULL ,10));
}

void	Server::parseServerName(std::ifstream& f)
{
	parse_single_str_value(f, server_name_);
}

void	Server::parseIndex(std::ifstream& f)
{
	parse_index(f, index_);
}

void	Server::parseRoot(std::ifstream& f)
{
	parse_single_str_value(f, root_);
}

void	Server::parseErrorPage(std::ifstream& f)
{
	parse_single_str_value(f, error_page_);
}

void	Server::parseBodySize(std::ifstream& f)
{
	parse_uint_value(f, body_size_);
}
