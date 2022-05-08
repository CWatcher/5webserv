#include "Location.hpp"

#include <sstream>

Location::Location(const AConfig& config, std::ifstream& f)
{
    parsePath(f);
    std::string str;
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
        else if (str == "directory_page")
            parseDirectoryPage(f);
        else if (str == "location")
            parseLocation(f);
        else
            throw std::logic_error("location: unknown directive \"" + str + "\"");
    }
    AConfig::completeConfig(config);
}

void    Location::parsePath(std::ifstream& f)
{
    std::string         str;
    std::getline(f >> std::ws, str, '{');
    std::stringstream   ss(str);

    if (str.empty())
        throw std::logic_error("location: empty path");
    ss >> path_;
    ss >> std::ws;
    if (!ss.eof())
        throw std::logic_error("location: multiple path");
}

void    Location::parseDirectoryPage(std::ifstream& f)
{
    std::string         str;
    std::getline(f >> std::ws, str, ';');
    std::stringstream   ss(str);

    if (str.empty())
        throw std::logic_error("directory_page: empty value");
    if (!directory_page_.empty())
        throw std::logic_error("directory_page: duplicate");
    ss >> directory_page_;
    ss >> std::ws;
    if (!ss.eof())
        throw std::logic_error("directory_page: too many values");
}

void    Location::parseLocation(std::ifstream& f)
{
    Location    new_location(*this, f);

    if (location_.find(new_location.path()) == location_.end())
        location_[new_location.path()] = new_location;
    else
        throw std::logic_error("location: \"" + new_location.path() + "\" duplicate");
}

std::ostream&   operator<<(std::ostream& o, const Location& l)
{
    o << l.path() << '{';
    for (std::map<std::string, Location>::const_iterator it = l.location().begin(); it != l.location().end(); ++it)
        o << it->second ;
    o << '}';
    return o;
}
