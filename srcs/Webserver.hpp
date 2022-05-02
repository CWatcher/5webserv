#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# include "Server.hpp"

class Webserver
{
public:
	Webserver();

	void						parseConfig(const char* filename);
private:
	void						parseRoot(std::ifstream& f);
	void						parseIndex(std::ifstream& f);
	void						parseAutoindex(std::ifstream& f);
	void						parseBodySize(std::ifstream& f);
	void						parseErrorPage(std::ifstream& f);
	// void						parseMethods(std::ifstream& f);
	void						parseServer(std::ifstream& f);
private:
	std::string								root_;
	std::vector<std::string>				index_;
	bool									autoindex_;
	unsigned								body_size_;
	std::string								error_page_;
	//char									methods_;
	// set/map of ports
	std::vector<Server>						servers_;

	std::ifstream							file_;

	typedef void							(Webserver::*f)(std::ifstream&);
	static const std::pair<std::string, f>	parser_init_list_[];
	const std::map<std::string, f>			parser_;		
};

# endif
