#ifndef CONFIG_PARSER_HPP
# define CONFIG_PARSER_HPP

# include "ServerConfig.hpp"

# include <fstream>

# define ROOT_DFL       "./www"
# define INDEX_DFL      "index.html"
# define AUTOINDEX_DFL  false
# define BODY_SIZE_DFL  0
# define METHODS_DFL    "GET POST DELETE"
# define HOST_DFL       "0.0.0.0"
# define PORT_DFL       80

class ConfigParser
{
public:
    ConfigParser(const char *filename);
    void                                                parse();
    const std::vector<ServerConfig>&                          getServers() const {return servers_;};
    const std::map<in_addr_t, std::set<in_port_t> >&    getListened() const {return listened_;};
private:
    void                        parseServer();
    void                        parseBlock(BaseConfig& block);

    void                        parseRoot(BaseConfig& parent);
    void                        parseIndex(BaseConfig& parent);
    void                        parseAutoindex(BaseConfig& parent);
    void                        parseErrorPage(BaseConfig& parent);
    void                        parseBodySize(BaseConfig& parent);
    void                        parseMethods(BaseConfig& parent);
    void                        parseReturn(BaseConfig& parent);
    void                        parseLocation(BaseConfig& parent);
    void                        parseListen(BaseConfig& parent);
    void                        parseServerName(BaseConfig& parent);

    std::vector<std::string>    getValues(char delim, const std::string& directive);
    std::string                 getValue(char delim, const std::string& directive);
    unsigned                    strToUInt(const std::string& str, const std::string& directive);

    static void                 completeServer(ServerConfig& server);
    static void                 completeLocation(const BaseConfig& parent, BaseConfig& location);
private:
    std::ifstream                               f_;
    std::string                                 block_;

    std::vector<ServerConfig>                         servers_;
    std::map<in_addr_t, std::set<in_port_t> >   listened_;

    typedef void (ConfigParser::*parser)(BaseConfig&);
    static const std::pair<std::string, parser> init_list_[];
    static const std::map<std::string, parser>  parsers_;
};

std::ostream&   operator<<(std::ostream& o, const ConfigParser& parser);

#endif
