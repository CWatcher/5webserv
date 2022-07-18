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
    std::make_pair("location", &ConfigParser::parseLocation),
    std::make_pair("listen", &ConfigParser::parseListen),
    std::make_pair("server_name", &ConfigParser::parseServerName)
};

const std::map<std::string, ConfigParser::parser> ConfigParser::parsers_(init_list_, init_list_ + sizeof(init_list_) / sizeof(init_list_[0]));

ConfigParser::ConfigParser(const char *filename) : block_("server")
{
    if (filename == NULL)
        filename = "config";
    f_.open(filename);
    if (f_.fail())
        throw std::logic_error(std::string("Unable to open file '") + filename + "'");
    f_.exceptions(std::ifstream::badbit);
}

void    ConfigParser::parse()
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
            parseServer();
        else
            throw std::logic_error("unknown directive '" + directive + "'");
    }
    logger::info << "configuration file OK" << logger::end;
    f_.close();
}

void    ConfigParser::parseServer()
{
    std::string str;
    Server      server;

    f_.exceptions(std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit);
    try
    {
        std::getline(f_ >> std::ws, str, '{');
        if (!str.empty())
            throw std::logic_error("server: unexpected '" + str + "'");
        parseBlock(server);
    }
    catch (const std::ifstream::failure& e)
    {
        if (f_.eof())
            throw std::logic_error(block_ + ": unexpected end of file");
        throw e;
    }
    f_.exceptions(std::ifstream::badbit);
    completeServer(server);
    servers_.push_back(server);
    for (std::map<in_addr_t, std::set<in_port_t> >::iterator it = server.listen.begin(); it != server.listen.end(); ++it)
        for (std::set<in_port_t>::iterator port = it->second.begin(); port != it->second.end(); ++port)
            listened_[it->first].insert(*port);
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
        server.methods.insert(std::istream_iterator<std::string> (ss), std::istream_iterator<std::string>());
    }
    if (server.listen.empty())
    {
        in_addr_t   host = inet_addr(HOST_DFL);
        in_port_t   port = htons(PORT_DFL);
        server.listen[host].insert(port);
    }
    for (std::map<std::string, Location>::iterator it = server.location.begin(); it != server.location.end(); ++it)
        completeLocation(server, it->second);
}

void    ConfigParser::completeLocation(const BaseConfig& parent, BaseConfig& location)
{
    if (location.root.empty())
        location.root = parent.root;
    if (location.index.empty())
        location.index = parent.index;
    if (location.autoindex == -1)
        location.autoindex = parent.autoindex;
    if (location.body_size == std::numeric_limits<unsigned>::max())
        location.body_size = parent.body_size;
    if (location.methods.empty())
        location.methods = parent.methods;
    for (std::map<unsigned, std::string>::const_iterator it = parent.error_page.begin(); it != parent.error_page.end(); ++it)
        if (location.error_page.find(it->first) == location.error_page.end())
            location.error_page[it->first] = it->second;
    for (std::map<std::string, Location>::iterator it = location.location.begin(); it != location.location.end(); ++it)
        completeLocation(location, it->second);
}

void    ConfigParser::parseBlock(BaseConfig& block)
{
    std::string                                     directive;
    std::map<std::string, parser>::const_iterator   parser;

    while ((f_ >> std::ws).peek() != '}')
    {
        if (f_.peek() == '#')
        {
            f_.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        f_ >> directive;
        parser = parsers_.find(directive);
        if (parser != parsers_.end())
            (this->*parser->second)(block);
        else
            throw std::logic_error(block_ + ": unknown directive '" + directive + "'");
    }
    f_.get();
}

void    ConfigParser::parseLocation(BaseConfig& parent)
{
    std::string block_save = block_;
    Location    location;

    location.path = getValue("location", '{');
    block_ = "location";
    parseBlock(location);
    block_ = block_save;
    if (parent.location.find(location.path) == parent.location.end())
        parent.location[location.path] = location;
    else
        throw std::logic_error("location: '" + location.path + "' duplicate");
}

void    ConfigParser::parseRoot(BaseConfig& parent)
{
    if (!parent.root.empty())
        throw std::logic_error(block_ + ": root duplicate");
    parent.root = getValue("root", ';');
}

void    ConfigParser::parseIndex(BaseConfig& parent)
{
    std::vector<std::string>    indexes = getValues("index");

    parent.index.insert(parent.index.end(), indexes.begin(), indexes.end());
}

void    ConfigParser::parseAutoindex(BaseConfig& parent)
{
    std::string value = getValue("autoindex", ';');

    if (parent.autoindex != -1)
        throw std::logic_error(block_ + ": autoindex duplicate");
    if (value == "on")
        parent.autoindex = true;
    else if (value == "off")
        parent.autoindex = false;
    else
        throw std::logic_error(block_ + ": autoindex bad value '" + value + "'");
}

void    ConfigParser::parseErrorPage(BaseConfig& parent)
{
    std::vector<std::string>    values = getValues("error_page");
    unsigned                    code;

    if (values.size() < 2)
        throw std::logic_error(block_ + ": error_page not enough values");
    for (std::vector<std::string>::iterator it = values.begin(); it != values.end() - 1; it++)
    {
        code = strToUInt(*it, "error_page");
        if (code < 400 || code > 599)
             throw std::logic_error(block_ + ": error_page bad value '" + *it + "'");
        parent.error_page[code] = values.back();
    }
}

void    ConfigParser::parseBodySize(BaseConfig& parent)
{
    std::string str = getValue("body_size", ';');
    unsigned    size = strToUInt(str, "body_size");

    if (parent.body_size != std::numeric_limits<unsigned>::max())
        throw std::logic_error(block_ + ": body_size duplicate");
    parent.body_size = size;
}

void    ConfigParser::parseMethods(BaseConfig& parent)
{
    std::vector<std::string>    methods = getValues("methods");

    for (std::vector<std::string>::iterator method = methods.begin(); method != methods.end(); method++)
    {
        if (*method != "GET" && *method != "POST" && *method != "DELETE")
            throw std::logic_error(block_ + ": methods bad value '" + *method + "'");
        parent.methods.insert(*method);
    }
}

void    ConfigParser::parseReturn(BaseConfig& parent)
{
    std::vector<std::string>    values = getValues("return");
    unsigned                    code = 302;

    if (values.size() > 2)
        throw std::logic_error(block_ + ": return too many values");
    if (values.size() == 2)
        code = strToUInt(values[0], "return");
    if (code < 301 || (code > 303 && code != 307 && code != 308))
        throw std::logic_error(block_ + ": return bad code '" + values[0] +"'");
    if (parent.redirect.second.empty())
    {
        parent.redirect.first = code;
        parent.redirect.second = values.back();
    }
}

void    ConfigParser::parseListen(BaseConfig& parent)
{
    if (block_ != "server")
        throw std::logic_error(block_ + ": unexpected 'listen'");

    Server&                     server = static_cast<Server&>(parent);
    std::vector<std::string>    values = getValues("listen");
    std::string                 host = HOST_DFL, port = values.back();

    if (values.size() > 2)
        throw std::logic_error("server: listen: too many values");
    if (values.size() == 2)
        host = values[0];
    in_addr_t in_host = inet_addr(host.c_str());
    if (in_host == INADDR_NONE)
        throw std::logic_error("server: listen bad value '" + host + "'");
    in_port_t in_port = htons(strToUInt(port, "server: listen:"));
    if (!server.listen[in_host].insert(in_port).second)
        throw std::logic_error("server: duplicate listen " + host + ':' + port);
}

void    ConfigParser::parseServerName(BaseConfig& parent)
{
    if (block_ != "server")
        throw std::logic_error(block_ + ": unexpected 'server_name'");

    Server&                     server = static_cast<Server&>(parent);
    std::vector<std::string>    server_names = getValues("server_name");

    server.server_name.insert(server.server_name.end(), server_names.begin(), server_names.end());
}

std::vector<std::string>    ConfigParser::getValues(const std::string& directive, char delim)
{
    std::string                 str;
    std::getline(f_ >> std::ws, str, delim);
    std::stringstream           ss(str);
    std::vector<std::string>    values((std::istream_iterator<std::string>(ss)), std::istream_iterator<std::string>());

    if (values.empty())
        throw std::logic_error(block_ + ": " + directive + " no values");
    return values;
}

std::string ConfigParser::getValue(const std::string& directive, char delim)
{
    std::string         str;
    std::getline(f_ >> std::ws, str, delim);
    std::stringstream   ss(str);

    if (str.empty())
        throw std::logic_error(block_ + ": " + directive + " no value");
    ss >> str;
    ss >> std::ws;
    if (!ss.eof())
        throw std::logic_error(block_ + ": " + directive + " too many values");
    return str;
}

unsigned    ConfigParser::strToUInt(const std::string& str, const std::string& directive)
{
    unsigned            number;
    std::stringstream   ss(str);

    if (str.find_first_not_of("0123456789") != std::string::npos)
         throw std::invalid_argument(block_ + ": " + directive + " bad value '" + str + "'");
    ss >> number;
    return number;
}

std::ostream&   operator<<(std::ostream& o, const ConfigParser& parser)
{
    for (size_t i = 0; i < parser.getServers().size(); i++)
    {
        o << "\033[0;32m" << std::string(10, '-') << "Server " << i << std::string(10, '-') << "\033[0m" << std::endl;
        o << parser.getServers()[i];
    }
    o << "\033[0;32m" << "Listened: "<< "\033[0m";
    in_addr tmp;
    for (std::map<in_addr_t, std::set<in_port_t> >::const_iterator it = parser.getListened().begin(); it != parser.getListened().end(); ++it)
    {
        tmp.s_addr = it->first;
        for (std::set<in_port_t>::const_iterator port = it->second.begin(); port != it->second.end(); ++port)
            o << inet_ntoa(tmp) << ":" << ntohs(*port) << ' ';
    }
    o << std::endl;
    return o;
}
