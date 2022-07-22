#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <string>
# include <vector>
# include <set>
# include <map>
# include <netinet/in.h>
# include <limits>

struct Location;

struct BaseConfig
{
    BaseConfig() : path("/"), autoindex(-1), body_size(std::numeric_limits<unsigned>::max()){}

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
    std::map<std::string, Location>     location;
};

struct Location : public BaseConfig
{
};

struct ServerConfig : public BaseConfig
{
    std::map<in_addr_t, std::set<in_port_t> >   listen;
    std::vector<std::string>                    server_name;
};

std::ostream&   operator<<(std::ostream& o, const ServerConfig& s);

#endif
