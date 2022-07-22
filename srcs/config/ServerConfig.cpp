#include "ServerConfig.hpp"
#include "utils/syntax.hpp"

#include <iostream>
#include <arpa/inet.h>

static std::ostream&    operator<<(std::ostream& o, const Location& l)
{
    o << l.path << '{';
    for (std::map<std::string, Location>::const_iterator it = l.location.begin(); it != l.location.end(); ++it)
        o << it->second ;
    o << '}';
    return o;
}

static std::ostream&    operator<<(std::ostream& o, const BaseConfig& c)
{
    o << "root: " << c.root;
    o << std::endl << "index: ";
    cforeach(std::vector<std::string>, c.index, it)
        o << *it << ' ';
    o << std::endl << "autoindex: " << std::boolalpha << c.autoindex;
    o << std::endl << "error_page: ";
    for (std::map<unsigned, std::string>::const_iterator it = c.error_page.begin(); it != c.error_page.end(); ++it)
        o << it->first << ':' << it->second << ' ';
    o << std::endl << "body_size: " << c.body_size;
    o << std::endl << "methods: ";
    cforeach(std::set<std::string>, c.methods, it)
        o << *it << ' ';
    o << std::endl << "directory_page: " << c.directory_page;
    o << std::endl << "upload_store: " << c.upload_store;
    o << std::endl << "redirect: ";
    if (!c.redirect.second.empty())
        o << c.redirect.first << ' ' << c.redirect.second;
    o << std::endl << "location: ";
    for (std::map<std::string, Location>::const_iterator it = c.location.begin(); it != c.location.end(); ++it)
    {
        o << it->second << ' ';
        // o << it->second.path << std::endl << static_cast<const BaseConfig>(it->second);
    }
    o << std::endl;
    return o;
}

std::ostream&   operator<<(std::ostream& o, const ServerConfig& s)
{
    o << static_cast<const BaseConfig&>(s);
    o << "sever_name: ";
    cforeach(std::vector<std::string>, s.server_name, it)
        o << *it << ' ';
    o << std::endl << "listen: " << std::endl;
    in_addr tmp;
    for (std::map<in_addr_t, std::set<in_port_t> >::const_iterator it = s.listen.begin(); it != s.listen.end(); ++it)
    {
        tmp.s_addr = it->first;
        o << "- " << inet_ntoa(tmp) << ": ";
        for (std::set<in_port_t>::const_iterator port = it->second.begin(); port != it->second.end(); ++port)
            o << ntohs(*port) << ' ';
        o << std::endl;
    }
    return o;
}
