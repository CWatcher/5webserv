#include "VirtualServer.hpp"
#include "utils/syntax.hpp"

#include <iostream>
#include <arpa/inet.h>
#include <iomanip>

const Location&   VirtualServer::getLocation(const VirtualServer& server, const std::string& uri)
{
    for (std::map<std::string, Location>::const_reverse_iterator rit = server.locations.rbegin();
        rit != server.locations.rend(); ++rit)
    {
        const std::string& path = rit->first;
        if (uri.compare(0, path.size(), path) == 0)
            return rit->second;
    }
    return server;
}

// static void    show_locations(std::ostream& o, const Location& l)
// {
//     o << l.path << '{';
//     for (std::map<std::string, Location>::const_iterator it = l.locations.begin(); it != l.locations.end(); ++it)
//         show_locations(o, it->second);
//     o << '}';
// }

static std::ostream&    operator<<(std::ostream& o, const Location& c)
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
    o << std::endl << "cgi:" << std::endl;
    for (std::map<std::string, std::string>::const_iterator it = c.cgi.begin(); it != c.cgi.end(); it++)
        o << "- " << it->first << ' ' << it->second << std::endl;
    return o;
}

std::ostream&   operator<<(std::ostream& o, const VirtualServer& s)
{
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
    o << static_cast<const Location&>(s);
    o << "\033[0;34m" << "locations:" << "\033[0m" << std::endl;
    for (std::map<std::string, Location>::const_iterator it = s.locations.begin(); it != s.locations.end(); ++it)
    {
        // o << "- " << std::setw(42) << std::left << it->first << " parent: " << it->second.parent << std::endl << it->second << std::endl;
        o << "path: " << std::setw(42) << std::left << it->first << " parent: " << it->second.parent << std::endl;
    }
    return o;
}
