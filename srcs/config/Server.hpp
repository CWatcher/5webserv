#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <vector>
# include <set>
# include <map>
# include <netinet/in.h>
# include <limits>

struct Location;

struct BaseConfig
{
    BaseConfig() : autoindex(-1), body_size(std::numeric_limits<unsigned>::max()){}

    std::string                         root;
    std::vector<std::string>            index;
    int                                 autoindex;
    std::map<unsigned, std::string>     error_page;
    unsigned                            body_size;
    std::set<std::string>               methods;
    std::pair<unsigned, std::string>    redirect;
    std::map<std::string, Location>     location;
};

struct Location : public BaseConfig
{
    std::string path;
};

struct Server : public BaseConfig
{
    std::map<in_addr_t, std::set<in_port_t> >   listen;
    std::vector<std::string>                    server_name;
};

std::ostream&   operator<<(std::ostream& o, const Server& s);

#endif
