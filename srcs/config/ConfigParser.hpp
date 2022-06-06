#ifndef CONFIG_PARSER_HPP
# define CONFIG_PARSER_HPP

# include "Server.hpp"

# include <fstream>

class ConfigParser
{
public:
    ConfigParser(const char *filename);
    void                        parseConfig();
    std::vector<Server>         getServers() const {return servers_;};
private:
    void                        parseServer(Server& server);

    void                        parseRoot(BaseConfig& config, const std::string& block);
    void                        parseIndex(BaseConfig& config, const std::string& block);
    void                        parseAutoindex(BaseConfig& config, const std::string& block);
    void                        parseErrorPage(BaseConfig& config, const std::string& block);
    void                        parseBodySize(BaseConfig& config, const std::string& block);
    void                        parseMethods(BaseConfig& config, const std::string& block);
    void                        parseReturn(BaseConfig& config, const std::string& block);
    void                        parseLocation(BaseConfig& config, const std::string& block);

    void                        parseListen(Server& server);
    void                        parseServerName(Server& server);

    void                        parseDirectoryPage(Location& location);

    std::vector<std::string>    getValues(char before, const std::string& block_directive);
    std::string                 getValue(char before, const std::string& block_directive);

    static unsigned             strToUInt(const std::string& str, const std::string& block_directive);
    static void                 completeServer(Server& server);
    static void                 completeLocation(const BaseConfig& parent, BaseConfig& location);
private:
    std::ifstream       f_;
    std::vector<Server> servers_;

    typedef void (ConfigParser::*parser)(BaseConfig&, const std::string&);
    static const std::pair<std::string, parser>   init_list_[];
    static const std::map<std::string, parser>    base_parsers_;
};

std::ostream&   operator<<(std::ostream& o, const ConfigParser& parser);

#endif
