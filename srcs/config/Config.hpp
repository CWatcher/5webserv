#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "ServerConfig.hpp"

# include <fstream>

# define ROOT_DFL           "./www"
# define INDEX_DFL          "index.html"
# define AUTOINDEX_DFL      false
# define BODY_SIZE_DFL      0
# define HOST_DFL           "0.0.0.0"
# define PORT_DFL           80

class Config
{
public:
    Config(const char* filename);
    const ServerConfig&                                 getServer(in_addr_t host, in_port_t port, const std::string& name);
    const std::map<in_addr_t, std::set<in_port_t> >&    getListened() const {return listened_;};

    friend std::ostream&                                operator<<(std::ostream& o, const Config& config);

    class bad_config : public std::runtime_error
    {
    public:
        bad_config(const std::string& msg) : runtime_error(msg) {}
    };
private:
    void                        parseConfig();
    void                        parseServer();
    void                        parseBlock(Location& block);

    void                        parseRoot(Location& parent);
    void                        parseIndex(Location& parent);
    void                        parseAutoindex(Location& parent);
    void                        parseErrorPage(Location& parent);
    void                        parseBodySize(Location& parent);
    void                        parseMethods(Location& parent);
    void                        parseReturn(Location& parent);
    void                        parseDirectoryPage(Location& parent);
    void                        parseUploadStore(Location& parent);
    void                        parseCgi(Location& parent);
    void                        parseLocation(Location& parent);
    void                        parseListen(Location& parent);
    void                        parseServerName(Location& parent);

    std::vector<std::string>    getValues(const char* directive, char delim = ';');
    std::string                 getValue(const char* directive, char delim = ';');
    unsigned                    strToUInt(const std::string& str, const char* directive);

    static void                 completeServer(ServerConfig& server);
    static void                 completeLocation(const Location& parent, Location& location);
private:
    std::ifstream                               f_;
    std::string                                 block_;

    std::vector<ServerConfig>                   servers_;
    std::map<in_addr_t, std::set<in_port_t> >   listened_;

    typedef void (Config::*parser)(Location&);
    static const std::pair<std::string, parser> init_list_[];
    static const std::map<std::string, parser>  parsers_;
};
#endif
