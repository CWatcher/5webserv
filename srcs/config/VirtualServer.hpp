#ifndef VIRTUALSERVER_HPP
# define VIRTUALSERVER_HPP

# include <string>
# include <vector>
# include <set>
# include <map>
# include <netinet/in.h>
# include <limits>

struct Location
{
    Location() : autoindex(-1), body_size(std::numeric_limits<unsigned>::max()){}

    std::string                         path;
    std::string                         root;
    std::vector<std::string>            index;
    int                                 autoindex;
    std::map<unsigned, std::string>     error_page;
    unsigned                            body_size;
    std::pair<unsigned, std::string>    redirect;
    std::set<std::string>               methods;
    std::string                         directory_page;
    std::string                         upload_store;
    std::map<std::string, std::string>  cgi;
    std::string                         parent;
};

struct VirtualServer : public Location
{
    std::map<in_addr_t, std::set<in_port_t> >   listen;
    std::vector<std::string>                    server_name;
    std::map<std::string, Location>             locations;
};

std::ostream&   operator<<(std::ostream& o, const VirtualServer& s);

#endif
