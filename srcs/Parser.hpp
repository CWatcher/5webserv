#ifndef PARSER_HPP
# define PARSER_HPP

# include "Server.hpp"

class Parser : public AConfig
{
public:
    Parser(const char* file_name);
    const std::set<Listen>&     getListen() const {return listen_;}
    const std::vector<Server>&  getServers() const {return servers_;}
private:
    void                        addServer(std::ifstream& f);
private:
    std::set<Listen>            listen_;
    std::vector<Server>         servers_;
};

std::ostream&   operator<<(std::ostream& o, const Parser& p);

# endif
