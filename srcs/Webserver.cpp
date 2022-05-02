#include "Webserver.hpp"
#include "parser_utils.hpp"

const std::pair<std::string, Webserver::f>	Webserver::parser_init_list_[] = {
	std::make_pair("root", &Webserver::parseRoot),
	std::make_pair("index", &Webserver::parseIndex),
	std::make_pair("autoindex", &Webserver::parseAutoindex),
	std::make_pair("body_size", &Webserver::parseBodySize),
	std::make_pair("error_page", &Webserver::parseErrorPage),
	std::make_pair("server", &Webserver::parseServer)
};

Webserver::Webserver() :
	root_("/var/www/"),
	index_(1, "index.html"),
	autoindex_(false),
	body_size_(0),
	error_page_(""),
	//ports, 
	servers_(1, Server()),
	parser_(parser_init_list_, parser_init_list_ + 1)
{}

void	Webserver::parseConfig(const char* filename)
{
	std::ifstream	file;
	std::string		token;

	file_.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	file_.open(filename);
	while (true)
	{
		file >> token;

		/* code */
	}
	file.close();
}

void	Webserver::parseRoot(std::ifstream& f)
{
	parse_single_str_value(f , root_);
}

void	Webserver::parseIndex(std::ifstream& f)
{
	parse_index(f, index_);
}

void	Webserver::parseAutoindex(std::ifstream& f)
{
	parse_autoindex(f, autoindex_);
}

void	Webserver::parseBodySize(std::ifstream& f)
{
	parse_uint_value(f, body_size_);
}

void	Webserver::parseErrorPage(std::ifstream& f)
{
	parse_single_str_value(f, error_page_);
}
