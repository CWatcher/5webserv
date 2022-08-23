#include "ServerConfig.hpp"
#include "utils/log.hpp"
#include "utils/string.hpp"

#include <stdexcept>
#include <sstream>
#include <iterator>
#include <arpa/inet.h>
#include <algorithm>

const std::pair<std::string, ServerConfig::parser> ServerConfig::init_list_[] =
{
    std::make_pair("root", &ServerConfig::parseRoot),
    std::make_pair("index", &ServerConfig::parseIndex),
    std::make_pair("autoindex", &ServerConfig::parseAutoindex),
    std::make_pair("error_page", &ServerConfig::parseErrorPage),
    std::make_pair("body_size", &ServerConfig::parseBodySize),
    std::make_pair("return", &ServerConfig::parseReturn),
    std::make_pair("methods", &ServerConfig::parseMethods),
    std::make_pair("upload_store", &ServerConfig::parseUploadStore),
    std::make_pair("cgi", &ServerConfig::parseCgi),
    std::make_pair("location", &ServerConfig::parseLocation),
    std::make_pair("listen", &ServerConfig::parseListen),
    std::make_pair("server_name", &ServerConfig::parseServerName)
};

const std::map<std::string, ServerConfig::parser> ServerConfig::parsers_(init_list_, init_list_ + sizeof(init_list_) / sizeof(init_list_[0]));

ServerConfig::ServerConfig(const char *filename) : block_("server:")
{
    if (filename == NULL)
        filename = "config";
    f_.open(filename);
    if (f_.fail())
        throw bad_config(std::string("unable to open file '") + filename + "'");
    logger::info << "configuration file '" << filename << "' opened" << logger::end;
    try{
        parseConfig();
    }
    catch (const std::exception& e)
    {
        f_.close();
        throw;
    }
    f_.close();
    logger::info << "configuration file '" << filename << "' loaded" << logger::end;
}

const VirtualServer& ServerConfig::getVirtualServer(in_addr_t host, in_port_t port, const std::string& name) const
{
    std::vector<VirtualServer>::const_iterator server = servers_.begin();

    for(std::vector<VirtualServer>::const_iterator it = servers_.begin(); it != servers_.end(); it++)
    {
        std::map<in_addr_t, std::set<in_port_t> >::const_iterator ports = it->listen.find(host);
        if (ports != it->listen.end() && ports->second.find(port) != ports->second.end())
        {
            if (std::find(it->server_name.begin(), it->server_name.end(), name) != it->server_name.end())
                return *it;
            if (server == servers_.end())
                server = it;
        }
    }
    return *server;
}

const std::map<in_addr_t, std::set<in_port_t> >    ServerConfig::getListened() const
{
    std::map<in_addr_t, std::set<in_port_t> >   listened;

    for (std::vector<VirtualServer>::const_iterator server = servers_.begin(); server != servers_.end(); server++)
        for (std::map<in_addr_t, std::set<in_port_t> >::const_iterator it = server->listen.begin(); it != server->listen.end(); ++it)
            for (std::set<in_port_t>::iterator port = it->second.begin(); port != it->second.end(); ++port)
                listened[it->first].insert(*port);
    return listened;
}

void    ServerConfig::parseConfig()
{
    std::string directive;

    f_.exceptions(std::ifstream::badbit);
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
            throw bad_config("unknown directive '" + directive + "'");
    }
}

void    ServerConfig::parseServer()
{
    std::string     str;
    VirtualServer   server;

    server_locations_ = &server.locations;
    f_.exceptions(std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit);
    try
    {
        std::getline(f_ >> std::ws, str, '{');
        if (!str.empty())
            throw bad_config("server: unexpected '" + str + "'");
        server.path = '/';
        parseBlock(server);
    }
    catch (const std::ifstream::failure& e)
    {
        if (f_.eof())
            throw bad_config(block_ + " unexpected end of file");
        throw;
    }
    f_.exceptions(std::ifstream::badbit);
    completeServer(server);
    servers_.push_back(server);

}

void    ServerConfig::completeServer(VirtualServer& server)
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
        server.methods.insert(METHOD_DFL);
    if (server.listen.empty())
    {
        in_addr_t   host = inet_addr(HOST_DFL);
        in_port_t   port = htons(PORT_DFL);
        server.listen[host].insert(port);
    }
    server.locations["/"] = server;
    for (std::map<std::string, Location>::iterator it = ++server.locations.begin(); it != server.locations.end(); ++it)
        completeLocation(server.locations[it->second.parent], it->second);
}

void    ServerConfig::completeLocation(const Location& parent, Location& location)
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
    if (location.upload_store.empty())
        location.upload_store = parent.upload_store;
    for (std::map<std::string, std::string>::const_iterator it = parent.cgi.begin(); it != parent.cgi.end(); ++it)
    {
        std::map<std::string, std::string>::const_iterator find = location.cgi.find(it->first);
        if (find == location.cgi.end())
            location.cgi[it->first] = it->second;
        else if (it->second != find->second)
            throw bad_config("location '" + location.path + "': cgi '" + find->first + "' path mismathc '" + it->second + "' and '" + find->second + "'");
    }
    for (std::map<unsigned, std::string>::const_iterator it = parent.error_page.begin(); it != parent.error_page.end(); ++it)
        if (location.error_page.find(it->first) == location.error_page.end())
            location.error_page[it->first] = it->second;
}

void    ServerConfig::parseBlock(Location& block)
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
            throw bad_config(block_ + " unknown directive '" + directive + "'");
    }
    f_.get();
}

void    ServerConfig::parseLocation(Location& parent)
{
    std::string block_save = block_;
    Location    location;

    location.path = getValue("location", '{');
    strRemoveDoubled(location.path, '/');
    block_ = "location '" + location.path + "':";
    if (location.path.compare(0, parent.path.size(), parent.path) != 0)
            throw bad_config(block_ + " is outside location '" + parent.path + "'");
    location.parent = parent.path;
    parseBlock(location);
    block_ = block_save;
    if (!server_locations_->insert(std::make_pair(location.path, location)).second)
        throw bad_config(block_ + " duplicate");
}

void    ServerConfig::parseRoot(Location& parent)
{
    if (!parent.root.empty())
        throw bad_config(block_ + " root duplicate");
    parent.root = getValue("root");
    strRemoveDoubled(parent.root, '/');
}

void    ServerConfig::parseIndex(Location& parent)
{
    std::vector<std::string>    indexes = getValues("index");

    parent.index.insert(parent.index.end(), indexes.begin(), indexes.end());
}

void    ServerConfig::parseAutoindex(Location& parent)
{
    std::string value = getValue("autoindex", ';');

    if (parent.autoindex != -1)
        throw bad_config(block_ + " autoindex duplicate");
    if (value == "on")
        parent.autoindex = true;
    else if (value == "off")
        parent.autoindex = false;
    else
        throw bad_config(block_ + " autoindex bad value '" + value + "'");
}

void    ServerConfig::parseErrorPage(Location& parent)
{
    std::vector<std::string>    values = getValues("error_page");
    unsigned                    code;

    if (values.size() < 2)
        throw bad_config(block_ + " error_page not enough values");
    for (std::vector<std::string>::iterator it = values.begin(); it != values.end() - 1; it++)
    {
        code = strToUInt(*it, "error_page");
        if (code < 400 || code > 599)
             throw bad_config(block_ + " error_page bad value '" + *it + "'");
        parent.error_page[code] = strRemoveDoubled(values.back(), '/');
    }
}

void    ServerConfig::parseBodySize(Location& parent)
{
    std::string str = getValue("body_size", ';');
    unsigned    size = strToUInt(str, "body_size");

    if (parent.body_size != std::numeric_limits<unsigned>::max())
        throw bad_config(block_ + " body_size duplicate");
    parent.body_size = size;
}

void    ServerConfig::parseMethods(Location& parent)
{
    std::vector<std::string>    methods = getValues("methods");

    for (std::vector<std::string>::iterator method = methods.begin(); method != methods.end(); method++)
    {
        if (*method != "GET" && *method != "POST" && *method != "DELETE")
            throw bad_config(block_ + " methods bad value '" + *method + "'");
        parent.methods.insert(*method);
    }
}

void    ServerConfig::parseUploadStore(Location& parent)
{
    if (!parent.upload_store.empty())
        throw bad_config(block_ + " upload_store duplicate");
    parent.upload_store = getValue("upload_store");
    strRemoveDoubled(parent.upload_store, '/');
}

void    ServerConfig::parseCgi(Location& parent)
{
    std::vector<std::string>    values = getValues("cgi");

    if (values.size() != 2)
        throw bad_config(block_ + " cgi need two values");
    if (!parent.cgi.insert(std::make_pair(values[0], strRemoveDoubled(values[1], '/'))).second)
        throw bad_config(block_ + " cgi '" + values[0] + "' duplicate");
}

void    ServerConfig::parseReturn(Location& parent)
{
    std::vector<std::string>    values = getValues("return");
    unsigned                    code = 302;

    if (values.size() > 2)
        throw bad_config(block_ + " return too many values");
    if (values.size() == 2)
        code = strToUInt(values[0], "return");
    if (code < 301 || (code > 303 && code != 307 && code != 308))
        throw bad_config(block_ + " return bad code '" + values[0] +"'");
    if (parent.redirect.second.empty())
    {
        parent.redirect.first = code;
        parent.redirect.second = values.back();
    }
}

void    ServerConfig::parseListen(Location& parent)
{
    if (block_ != "server:")
        throw bad_config(block_ + " unexpected 'listen'");

    VirtualServer&              server = static_cast<VirtualServer&>(parent);
    std::vector<std::string>    values = getValues("listen");
    std::string                 host = HOST_DFL, port = values.back();

    if (values.size() > 2)
        throw bad_config("server: listen too many values");
    if (values.size() == 2)
        host = values[0];
    in_addr_t in_host = inet_addr(host.c_str());
    if (in_host == INADDR_NONE)
        throw bad_config("server: listen bad value '" + host + "'");
    in_port_t in_port = htons(strToUInt(port, "server: listen:"));
    if (!server.listen[in_host].insert(in_port).second)
        throw bad_config("server: duplicate listen " + host + ':' + port);
}

void    ServerConfig::parseServerName(Location& parent)
{
    if (block_ != "server:")
        throw bad_config(block_ + " unexpected 'server_name'");

    VirtualServer&              server = static_cast<VirtualServer&>(parent);
    std::vector<std::string>    server_names = getValues("server_name");

    server.server_name.insert(server.server_name.end(), server_names.begin(), server_names.end());
}

std::vector<std::string>    ServerConfig::getValues(const char* directive, char delim)
{
    std::string                 str;
    std::getline(f_ >> std::ws, str, delim);
    std::stringstream           ss(str);
    std::vector<std::string>    values((std::istream_iterator<std::string>(ss)), std::istream_iterator<std::string>());

    if (values.empty())
        throw bad_config(block_ + " " + directive + " no values");
    return values;
}

std::string ServerConfig::getValue(const char* directive, char delim)
{
    std::string         str;
    std::getline(f_ >> std::ws, str, delim);
    std::stringstream   ss(str);

    if (str.empty())
        throw bad_config(block_ + " " + directive + " no value");
    ss >> str;
    ss >> std::ws;
    if (!ss.eof())
        throw bad_config(block_ + " " + directive + " too many values");
    return str;
}

unsigned    ServerConfig::strToUInt(const std::string& str, const char* directive)
{
    unsigned            number;
    std::stringstream   ss(str);

    if (str.find_first_not_of("0123456789") != std::string::npos)
        throw bad_config(block_ + " " + directive + " bad value '" + str + "'");
    ss >> number;
    return number;
}

std::ostream&   operator<<(std::ostream& o, const ServerConfig& config)
{
    for (size_t i = 0; i < config.servers_.size(); i++)
    {
        o << "\033[0;32m" << std::string(10, '-') << "Server " << i << std::string(10, '-') << "\033[0m" << std::endl;
        o << config.servers_[i];
    }
    o << "\033[0;32m" << "Listened: "<< "\033[0m";
    std::map<in_addr_t, std::set<in_port_t> >   listened = config.getListened();
    for (std::map<in_addr_t, std::set<in_port_t> >::iterator it = listened.begin(); it != listened.end(); ++it)
    {
        in_addr tmp;
        tmp.s_addr = it->first;
        for (std::set<in_port_t>::iterator port = it->second.begin(); port != it->second.end(); ++port)
            o << inet_ntoa(tmp) << ":" << ntohs(*port) << ' ';
    }
    o << std::endl;
    return o;
}
