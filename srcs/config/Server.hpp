#ifndef SERVER_HPP
# define SERVER_HPP

# include "Location.hpp"

# include <netinet/in.h>
# include <arpa/inet.h>

struct Listen
{
    Listen() : host(INADDR_NONE), port(0) {}
    in_addr_t   host;
    in_port_t   port;
    bool        operator<(const Listen& other) const {return host < other.host || (!(other.host < host) && port < other.port);}
};

class Server : public AConfig
{
public:
    Server(const AConfig& config, std::ifstream& file);

    const Listen&                           listen() const {return listen_;}
    const std::set<std::string>&            server_name() const  {return server_name_;}
    const std::map<std::string, Location>&  location() const {return location_;}
private:
    void                                    parseServerName(std::ifstream& f);
    void                                    parseHost(std::ifstream& f);
    void                                    parsePort(std::ifstream& f);
    void                                    parseLocation(std::ifstream& f);

    void                                    completeConfig(const AConfig& config);
private:
    Listen                                  listen_;
    std::set<std::string>                   server_name_;
    std::map<std::string, Location>         location_;
};

std::ostream&   operator<<(std::ostream& o, const Server& s);

#endif
