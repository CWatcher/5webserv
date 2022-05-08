#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "Server.hpp"

class Config : public AConfig
{
public:
    Config(const char* file_name);
    const std::vector<Server>&  getServers() const {return servers_;}
private:
    std::vector<Server>         servers_;
};

std::ostream&   operator<<(std::ostream& o, const Config& p);

# endif
