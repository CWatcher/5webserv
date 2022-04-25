#include <string>
#include <map>

struct Location
{
	/* data */
};


struct Cgi
{
	/* data */
};


struct Server
{
	std::string						server_name;
	std::string						host_str;
	// ???			host;
	int								port;
	std::string						root;
	std::string						error_page;
	unsigned						body_size;
	std::map<std::string, Location>	route;
	std::map<std::string, Cgi>		cgi;
	/* data */
};
