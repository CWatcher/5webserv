#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <vector>
# include <set>
# include <map>
# include <netinet/in.h>
# include <limits>

# define ROOT_DFL       "./www"
# define INDEX_DFL      "index.html"
# define AUTOINDEX_DFL  false
# define BODY_SIZE_DFL  0
# define METHODS_DFL    "GET POST DELETE"
# define HOST_DFL       "0.0.0.0"
# define PORT_DFL       80

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
    std::string directory_page;
    std::string path;
};

struct Server : public BaseConfig
{
    std::map<in_addr_t, std::set<in_port_t> >   listen;
    std::vector<std::string>                    server_name;
};

std::ostream&   operator<<(std::ostream& o, const Server& s);

// struct Listen
// {
//     Listen() : host(INADDR_NONE), port(0) {}
//     in_addr_t   host;
//     in_port_t   port;
//     bool        operator<(const Listen& other) const {return host < other.host || (!(other.host < host) && port < other.port);}
// };

// class Server : public AConfig
// {
// public:
//     Server(const AConfig& config, std::ifstream& file);

//     const Listen&                           listen() const {return listen_;}
//     const std::set<std::string>&            server_name() const  {return server_name_;}
//     const std::map<std::string, Location>&  location() const {return location_;}
// private:
//     void                                    parseServerName(std::ifstream& f);
//     void                                    parseHost(std::ifstream& f);
//     void                                    parsePort(std::ifstream& f);
//     void                                    parseLocation(std::ifstream& f);

//     void                                    completeConfig(const AConfig& config);
// private:
//     Listen                                  listen_;
//     std::set<std::string>                   server_name_;
//     std::map<std::string, Location>         location_;
// };


#endif
