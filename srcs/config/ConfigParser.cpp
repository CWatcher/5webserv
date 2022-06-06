#include "ConfigParser.hpp"
#include "utils/log.hpp"

#include <stdexcept>
#include <sstream>
#include <iterator>
#include <arpa/inet.h>

const std::pair<std::string, ConfigParser::parser> ConfigParser::init_list_[] =
{
    std::make_pair("root", &ConfigParser::parseRoot),
    std::make_pair("index", &ConfigParser::parseIndex),
    std::make_pair("autoindex", &ConfigParser::parseAutoindex),
    std::make_pair("error_page", &ConfigParser::parseErrorPage),
    std::make_pair("body_size", &ConfigParser::parseBodySize),
    std::make_pair("methods", &ConfigParser::parseMethods),
    std::make_pair("return", &ConfigParser::parseReturn),
    std::make_pair("location", &ConfigParser::parseLocation)
};

const std::map<std::string, ConfigParser::parser> ConfigParser::base_parsers_(ConfigParser::init_list_, ConfigParser::init_list_ + 8);


ConfigParser::ConfigParser(const char *filename)
{
    if (filename == NULL)
        filename = "config";
    f_.exceptions(std::ifstream::failbit);
    f_.open(filename);
    f_.exceptions(std::ifstream::badbit);
}

void    ConfigParser::parseConfig()
{
    std::string directive;

    while (!(f_ >> std::ws).eof())
    {
        if (f_.peek() == '#')
        {
            f_.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        f_ >> directive;
        if (directive == "server")
        {
            Server  new_server;
            f_.exceptions(std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit);
            parseServer(new_server);
            f_.exceptions(std::ifstream::badbit);
            servers_.push_back(new_server);
        }
        else
            throw std::logic_error("unknown directive \"" + directive + "\"");
    }
    logger::info << "configuration file OK" << logger::end;
    f_.close();
}

void    ConfigParser::parseServer(Server& server)
{
    std::string                                     directive;
    std::map<std::string, parser>::const_iterator   parser;

    std::getline(f_ >> std::ws, directive, '{');
    if (!directive.empty())
        throw std::logic_error("server: unexpected \"" + directive + "\"");
    while ((f_ >> std::ws).peek() != '}')
    {
        if (f_.peek() == '#')
        {
            f_.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        f_ >> directive;
        parser = base_parsers_.find(directive);
        if (parser != base_parsers_.end())
            (this->*parser->second)(server, "server");
        else if (directive == "listen")
            parseListen(server);
        else if (directive == "server_name")
            parseServerName(server);
        else
            throw std::logic_error("server: unknown directive \"" + directive + "\"");
    }
    f_.get();
    completeServer(server);
}

void    ConfigParser::parseLocation(BaseConfig& config, const std::string&)
{
    std::string                                     directive;
    std::map<std::string, parser>::const_iterator   parser;
    Location                                        new_location;

    new_location.path = getValue('{', "location: ");
    while ((f_ >> std::ws).peek() != '}')
    {
        if (f_.peek() == '#')
        {
            f_.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        f_ >> directive;
        parser = base_parsers_.find(directive);
        if (parser != base_parsers_.end())
            (this->*parser->second)(new_location, "location");
        else if (directive == "directory_page")
            parseDirectoryPage(new_location);
        else
            throw std::logic_error("location: unknown directive \"" + directive + "\"");
    }
    f_.get();
    completeLocation(config, new_location);
    if (config.location.find(new_location.path) == config.location.end())
        config.location[new_location.path] = new_location;
    else
        throw std::logic_error("location: \"" + new_location.path + "\" duplicate");
}

void    ConfigParser::parseRoot(BaseConfig& config, const std::string& block)
{
    if (!config.root.empty())
        throw std::logic_error(block + ": root duplicate");
    config.root = getValue(';', block + ": root");
}

void    ConfigParser::parseIndex(BaseConfig& config, const std::string& block)
{
    std::vector<std::string>    indexes = getValues(';', block + ": index");

    config.index.insert(config.index.end(), indexes.begin(), indexes.end());
}

void    ConfigParser::parseAutoindex(BaseConfig& config, const std::string& block)
{
    std::string value = getValue(';', block + ": autoindex");

    if (config.autoindex != -1)
        throw std::logic_error(block + ": autoindex duplicate");
    if (value == "on")
        config.autoindex = true;
    else if (value == "off")
        config.autoindex = false;
    else
        throw std::logic_error(block + ": autoindex bad value \"" + value + "\"");
}

void    ConfigParser::parseErrorPage(BaseConfig& config, const std::string& block)
{
    std::vector<std::string>    values = getValues(';', block + ": error_page");
    unsigned                    code;

    if (values.size() < 2)
        throw std::logic_error(block + " error_page: not enough values");
    for (std::vector<std::string>::iterator it = values.begin(); it != values.end() - 1; it++)
    {
        code = strToUInt(*it, block + ": error_page");
        if (code < 100 || code > 599)
             throw std::logic_error(block + ": error_page: bad value \"" + *it + "\"");
        config.error_page[code] = values.back();
    }
}

void    ConfigParser::parseBodySize(BaseConfig& config, const std::string& block)
{
    std::string str = getValue(';', block + ": body_size");
    unsigned    size = strToUInt(str, block + ": body_size");

    if (config.body_size != std::numeric_limits<unsigned>::max())
        throw std::logic_error(block + ": body_size duplicate");
    config.body_size = size;
}

void    ConfigParser::parseMethods(BaseConfig& config, const std::string& block)
{
    std::vector<std::string>    methods = getValues(';', block + ": methods");

    for (std::vector<std::string>::iterator method = methods.begin(); method != methods.end(); method++)
    {
        if (*method != "GET" && *method != "POST" && *method != "DELETE")
            throw std::logic_error(block + ": methods: bad value \"" + *method + "\"");
        config.methods.insert(*method);
    }
}

void    ConfigParser::parseReturn(BaseConfig& config, const std::string& block)
{
    std::vector<std::string>    values = getValues(';', block + ": return");
    unsigned                    code = 302;

    if (values.size() > 2)
        throw std::logic_error(block + " return: too many values");
    if (values.size() == 2)
        code = strToUInt(values[0], block + ": return");
    if (code < 301 || (code > 303 && code != 307 && code != 308))
        throw std::logic_error(block + ": return: bad code \"" + values[0] +"\"");
    if (config.redirect.second.empty())
    {
        config.redirect.first = code;
        config.redirect.second = values.back();
    }
}

void    ConfigParser::parseListen(Server& server)
{
    std::vector<std::string>    values = getValues(';', "server: listen");
    std::string                 host = HOST_DFL, port = values.back();

    if (values.size() > 2)
        throw std::logic_error("server: listen: too many values");
    if (values.size() == 2)
        host = values[0];
    in_addr_t in_host = inet_addr(host.c_str());
    if (in_host == INADDR_NONE)
        throw std::logic_error("server: listen: bad value \"" + host + "\"");
    in_port_t in_port = htons(strToUInt(port, "server: listen:"));
    if (!server.listen[in_host].insert(in_port).second)
        throw std::logic_error("server: duplicate listen " + host + ':' + port);
}

void    ConfigParser::parseServerName(Server& server)
{
    std::vector<std::string>    server_names = getValues(';', "server: server_name");

    server.server_name.insert(server.server_name.end(), server_names.begin(), server_names.end());
}


void   ConfigParser::parseDirectoryPage(Location& location)
{
    std::string page = getValue(';', "location: directory_page");

    if (!location.directory_page.empty())
        throw std::logic_error("location: body_size duplicate");
    location.directory_page = page;
}

std::vector<std::string>    ConfigParser::getValues(char before, const std::string& block_directive)
{
    std::string                 str;
    std::getline(f_ >> std::ws, str, before);
    std::stringstream           ss(str);
    std::vector<std::string>    values((std::istream_iterator<std::string>(ss)), std::istream_iterator<std::string>());

    if (values.empty())
        throw std::logic_error(block_directive + " no values");
    return values;
}

std::string ConfigParser::getValue(char before, const std::string& block_directive)
{
    std::string         str;
    std::getline(f_ >> std::ws, str, before);
    std::stringstream   ss(str);

    if (str.empty())
        throw std::logic_error(block_directive + " no value");
    ss >> str;
    ss >> std::ws;
    if (!ss.eof())
        throw std::logic_error(block_directive + " too many values");
    return str;
}

unsigned    ConfigParser::strToUInt(const std::string& str, const std::string& block_directive)
{
    unsigned            number;
    std::stringstream   ss(str);

    if (str.find_first_not_of("0123456789") != std::string::npos)
         throw std::invalid_argument(block_directive + ": bad value \"" + str + "\"");
    ss >> number;
    return number;
}

void    ConfigParser::completeServer(Server& server)
{
    if (server.root.empty())
        server.root = ROOT_DFL;
    if (server.index.empty())
        server.index.push_back(INDEX_DFL);
    if (server.autoindex == -1)
        server.autoindex = AUTOINDEX_DFL;
    if (server.body_size == std::numeric_limits<unsigned>::max())
        server.body_size = BODY_SIZE_DFL;
    if (server.methods.empty())
    {
        std::stringstream ss(METHODS_DFL);
        for (std::istream_iterator<std::string> method(ss); method != std::istream_iterator<std::string>(); ++method)
            server.methods.insert(*method);
    }
    if (server.listen.empty())
    {
        in_addr_t   host = inet_addr(HOST_DFL);
        in_port_t   port = htons(PORT_DFL);
        server.listen[host].insert(port);
    }
}

void    ConfigParser::completeLocation(const BaseConfig& parent, BaseConfig& location)
{
    if (location.root.empty())
        location.root = parent.root.empty() ? ROOT_DFL : parent.root;
    if (location.index.empty())
    {
        if (parent.index.empty())
            location.index.push_back(INDEX_DFL);
        else
            location.index = parent.index;
    }
    if (location.autoindex == -1)
        location.autoindex = parent.autoindex == -1 ? AUTOINDEX_DFL : parent.autoindex;
    if (location.body_size == std::numeric_limits<unsigned>::max())
        location.body_size = parent.body_size == std::numeric_limits<unsigned>::max() ? BODY_SIZE_DFL : parent.body_size;
    if (location.methods.empty())
    {
        if (parent.methods.empty())
        {
            std::stringstream ss(METHODS_DFL);
            for (std::istream_iterator<std::string> method(ss); method != std::istream_iterator<std::string>(); ++method)
                location.methods.insert(*method);
        }
        else
            location.methods = parent.methods;
    }
    for (std::map<unsigned, std::string>::const_iterator it = parent.error_page.begin(); it != parent.error_page.end(); ++it)
        if (location.error_page.find(it->first) == location.error_page.end())
            location.error_page[it->first] = it->second;
}

std::ostream&   operator<<(std::ostream& o, const ConfigParser& parser)
{
    for (size_t i = 0; i < parser.getServers().size(); i++)
    {
        o << "\033[0;32m" << std::string(10, '-') << "Server " << i << std::string(10, '-') << "\033[0m" << std::endl;
        o << parser.getServers()[i];
    }
    return o;
}
