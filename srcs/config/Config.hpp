#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "ServerConfig.hpp"

# include <fstream>

# define ROOT_DFL       "./www"
# define INDEX_DFL      "index.html"
# define AUTOINDEX_DFL  false
# define BODY_SIZE_DFL  0
# define METHODS_DFL    "GET POST DELETE"
# define HOST_DFL       "0.0.0.0"
# define PORT_DFL       80

class Config
{
public:
    Config(const char* filename);
    const ServerConfig&                                 getServer(in_addr_t host, in_port_t port, const std::string& name);
    const std::map<in_addr_t, std::set<in_port_t> >&    getListened() const {return listened_;};

    friend std::ostream&                                operator<<(std::ostream& o, const Config& parser);

    class bad_config : public std::runtime_error
    {
    public:
        bad_config(const std::string& msg) : runtime_error(msg) {}
    };
private:
    void                        parseConfig();
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

    std::vector<std::string>    getValues(const char* directive, char delim = ';');
    std::string                 getValue(const char* directive, char delim = ';');
    unsigned                    strToUInt(const std::string& str, const char* directive);

    static void                 completeServer(ServerConfig& server);
    static void                 completeLocation(const BaseConfig& parent, BaseConfig& location);
private:
    std::ifstream                               f_;
    std::string                                 block_;

    std::vector<ServerConfig>                   servers_;
    std::map<in_addr_t, std::set<in_port_t> >   listened_;

    typedef void (Config::*parser)(BaseConfig&);
    static const std::pair<std::string, parser> init_list_[];
    static const std::map<std::string, parser>  parsers_;
};
#endif
