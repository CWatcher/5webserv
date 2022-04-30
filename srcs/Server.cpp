#include "Server.hpp"
#include "utils/log.hpp"//

#include <exception>//
#include <sstream>//

const std::pair<std::string, Server::f>	Server::parser_init_list_[] = {
	// std::make_pair("listen", &Server::parseListen),
	std::make_pair("server_name", &Server::parseServerName),
	// std::make_pair("root", &Server::parseRoot)
	// std::make_pair("listen", &Server::parseIndex),
	// std::make_pair("listen", &Server::parseErrorPage),
	// std::make_pair("listen", &Server::parseBodySize),

};

const std::map<std::string, Server::f>	Server::parser(parser_init_list_, parser_init_list_ + 2);

Server::Server() :
    host_str_("0.0.0.0"),
    port_(8080),
    error_page_("error_"), //??
    body_size_(0)
{
	// ???
}

void	Server::readConfig(std::ifstream& file)
{
	std::string											token;
	std::map<std::string, Server::f>::const_iterator	it;

	file >> token;
	if (token != "{")
		throw std::logic_error("configuration server block error");
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
			break;
		it = parser.find(token);
		if (it == parser.end())
			throw std::logic_error( "unsupported server option");
		else
			(this->*it->second)(file);
	}

	// "unexpected end of configuration file"

	// while (file >> token);
	// {
    //         else if (token == "listen")
    //         {

    //         }
    //         else if (token == "root")
    //         {

    //         }
    //         else if (token == "index")
	// 		{

	// 		}
    //         else if (token == "server_name")
	// 		{

	// 		}
    //         else if (token == "location")
	// 		{

	// 		}
	// 		else if (token == "listen")
    //         {

    //         }
	// }
	// logger::error << "unexpected end of configuration file" << logger::end;
	// throw std::runtime_error("");
}

#include <iostream>
void	Server::parseServerName(std::ifstream& f)
{
	std::string			line;
	std::stringstream	ss;

	std::getline(f, line, ';');
	ss.str(line);
	ss >> server_name_;
	if (server_name_.empty())
		throw std::logic_error("empty server_name");
	ss >> std::ws;
	if (ss.peek() != std::char_traits<char>::eof())
		throw std::logic_error("");
}

void	Server::parseRoot(std::ifstream& f)
{
	f >> root_;
	std::cout << root_ << std::endl;
}
