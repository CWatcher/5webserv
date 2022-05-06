#include "Server.hpp"
#include "utils/syntax.hpp"

#include <exception>
#include <sstream>
#include <cstdlib>
#include <iterator>

Server::Server(const AConfig& config, std::ifstream& f)
{
    std::string str;

    f >> str;
    if (str != "{")
        throw std::logic_error("server: unexpected \"" + str + "\"");
    while (true)
    {
        if ((f >> std::ws).peek() == '#')
        {
            getline(f, str);
            continue;
        }
        f >> str;
        if (str == "}")
            break;
        else if (str == "root")
            parseRoot(f);
        else if (str == "index")
            parseIndex(f);
        else if (str == "autoindex")
            parseAutoindex(f);
        else if (str == "error_page")
            parseErrorPage(f);
        else if (str == "body_size")
            parseBodySize(f);
        else if (str == "methods")
            parseMethods(f);
        else if (str == "server_name")
            parseServerName(f);
        else if (str == "host")
            parseHost(f);
        else if (str == "port")
            parsePort(f);
        else if (str == "location")
            parseLocation(f);
        else
            throw std::logic_error("server: undefined directive \"" + str + "\"");
    }
    completeConfig(config);
}

void    Server::completeConfig(const AConfig& config)
{
    AConfig::completeConfig(config);
    if (listen_.host == INADDR_NONE)
        listen_.host = INADDR_ANY;
    if (listen_.port == 0)
        listen_.port = htons(80);
}

void    Server::parseServerName(std::ifstream& f)
{
    std::string         str;
    std::getline(f >> std::ws, str, ';');
    std::stringstream   ss(str);

    if (str.empty())
        throw std::logic_error("server_name: empty value");
    if (!server_name_.empty())
        throw std::logic_error("server_name: duplicated");
    server_name_.insert(std::istream_iterator<std::string>(ss), std::istream_iterator<std::string>());
}

void    Server::parseHost(std::ifstream& f)
{
    std::string                 str;
    std::getline(f >> std::ws, str, ';');
    std::stringstream           ss(str);

    if (str.empty())
        throw std::logic_error("host: empty value");
    if (listen_.host != INADDR_NONE)
        throw std::logic_error("host: duplicated");
    ss >> str;
    listen_.host = inet_addr(str.c_str());
    if (listen_.host == INADDR_NONE)
        throw std::logic_error("host: bad host \"" + str + "\"");
    ss >> std::ws;
    if (!ss.eof())
        throw std::logic_error("host: too many values");
}

void    Server::parsePort(std::ifstream& f)
{
    std::string         str;
    std::getline(f >> std::ws, str, ';');
    std::stringstream   ss(str);

    if (str.empty())
        throw std::logic_error("port: empty value");
    if (listen_.port != 0)
        throw std::logic_error("port: duplicated");
    int p;
    ss >> p;
    if (!isdigit(str[0]) || (!ss.eof() && !std::isspace(ss.peek())) || p < 1 || p > 65535)
        throw std::logic_error("port: bad port \"" + str + "\"");
    ss >> std::ws;
    if (!ss.eof())
        throw std::logic_error("port: too many values");
    listen_.port = htons(p);
}

void    Server::parseLocation(std::ifstream& f)
{
    (void)f;
}

std::ostream&   operator<<(std::ostream& o, const Server& s)
{
    o << static_cast<const AConfig&>(s);
    o << "sever_name: ";
    cforeach(std::set<std::string>, s.server_name(), it)
        o << *it << ' ';
    in_addr tmp;
    tmp.s_addr = s.listen().host;
    o << std::endl << "host: " << inet_ntoa(tmp);
    o << std::endl << "port: " << ntohs(s.listen().port) << std::endl;
    //location;
    return o;
}
