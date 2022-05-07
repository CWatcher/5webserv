#include "AConfig.hpp"
#include "utils/syntax.hpp"

#include <sstream>
#include <iterator>
#include <limits>

AConfig::AConfig() :
    root_(),
    index_(),
    autoindex_(-1),
    error_page_(),
    body_size_(std::numeric_limits<unsigned>::max()),
    methods_()
{}

AConfig::~AConfig() {}

void    AConfig::completeConfig(const AConfig& config)
{
    if (root_.empty())
        root_ = config.root_.empty() ? "/var/lib/www/html" : config.root_;
    if (index_.empty())
    {
        if (config.index_.empty())
            index_.push_back("index.html");
        else
            index_ = config.index_;
    }
    if (autoindex_ == -1)
        autoindex_ = config.autoindex_ == -1 ? false : config.autoindex_;
    if (body_size_ == std::numeric_limits<unsigned>::max())
        body_size_ = config.body_size_ == std::numeric_limits<unsigned>::max() ? 0 : config.body_size_;
    if (methods_.empty())
    {
        if (config.methods_.empty())
        {
            methods_.insert("GET");
            methods_.insert("POST");
            methods_.insert("DELETE");
        }
        else
            methods_ = config.methods_;
    }

    for (std::map<int, std::string>::const_iterator it = config.error_page_.begin(); it != config.error_page_.end(); ++it)
        if (error_page_.find(it->first) == error_page_.end())
            error_page_[it->first] = it->second;
}

void    AConfig::parseRoot(std::ifstream& f)
{

    std::string         str;
    std::getline(f >> std::ws, str, ';');
    std::stringstream   ss(str);

    if (str.empty())
        throw std::logic_error("root: empty value");
    if (!root_.empty())
        throw std::logic_error("root: duplicate");
    ss >> root_;
    ss >> std::ws;
    if (!ss.eof())
        throw std::logic_error("root: too many values");
}

void    AConfig::parseIndex(std::ifstream& f)
{
    std::string         str;
    std::getline(f >> std::ws, str, ';');
    std::stringstream   ss(str);

    if (str.empty())
        throw std::logic_error("index: empty value");
    if (!index_.empty())
        throw std::logic_error("index: duplicate");
    index_.insert(index_.end(), std::istream_iterator<std::string>(ss), std::istream_iterator<std::string>());
}

void    AConfig::parseAutoindex(std::ifstream& f)
{
    std::string         str;
    std::getline(f >> std::ws, str, ';');
    std::stringstream   ss(str);

    if (str.empty())
        throw std::logic_error("autoindex: empty value");
    if (autoindex_ != -1)
        throw std::logic_error("autoindex: duplicate");
    ss >> str;
    if (str == "on")
        autoindex_ = true;
    else if (str == "off")
        autoindex_ = false;
    else
        throw std::logic_error("autoindex: bad value \"" + str + "\"");
    ss >> std::ws;
    if (!ss.eof())
        throw std::logic_error("autoindex: too many values");
}

void    AConfig::parseErrorPage(std::ifstream& f)
{
    std::string                 str;
    std::getline(f >> std::ws, str, ';');
    std::stringstream           ss(str);
    std::vector<std::string>    splitted((std::istream_iterator<std::string>(ss)), std::istream_iterator<std::string>());

    if (splitted.size() < 2)
        throw std::logic_error("error_page: not enough values");
    int code;
    for (std::vector<std::string>::const_iterator it = splitted.begin(); it < splitted.end() - 1; ++it)
    {
        ss.clear();
        ss.str(*it);
        ss >> code;
        if (!isdigit((*it)[0]) || !ss.eof() || code < 100 || code > 599)
            throw std::logic_error("error_page: bad value \"" + *it + "\"");
        error_page_[code] = splitted.back();
    }
}
#include <iostream>
void    AConfig::parseBodySize(std::ifstream& f)
{
    std::string         str;
    std::getline(f >> std::ws, str, ';');
    std::stringstream   ss(str);

    if (str.empty())
        throw std::logic_error("body_size: empty value");
    if (body_size_ != std::numeric_limits<unsigned>::max())
        throw std::logic_error("body_size: duplicate");
    ss >> body_size_;
    if (!isdigit(str[0]) || (!ss.eof() && !std::isspace(ss.peek())))
        throw std::logic_error("body_size: bad value \"" + str + "\"");
    ss >> std::ws;
    if (!ss.eof())
        throw std::logic_error("body_size: too many values");
}

void    AConfig::parseMethods(std::ifstream& f)
{
    std::string         str;
    std::getline(f >> std::ws, str, ';');
    std::stringstream   ss(str);

    if (str.empty())
        throw std::logic_error("methods: empty value");
    if (!methods_.empty())
        throw std::logic_error("methods: duplicate");
    while (ss >> str)
    {
        if (str != "GET" && str != "POST" && str != "DELETE")
            throw std::logic_error("methods: bad value \"" + str + "\"");
        methods_.insert(str);
    }
}

std::ostream&   operator<<(std::ostream& o, const AConfig& c)
{
    o << "root: " << c.root();
    o << std::endl << "index: ";
    cforeach(std::vector<std::string>, c.index(), it)
        o << *it << ' ';
    o << std::endl << "autoindex: " << std::boolalpha << c.autoindex();
    o << std::endl << "error_page: ";
    for (std::map<int, std::string>::const_iterator it = c.error_page().begin(); it != c.error_page().end(); ++it)
        o << it->first << ':' << it->second << ' ';
    // cforeach((std::map<int, std::string>), c.error_page(), it)
        // o << it->first << ':' << it->second << ' ';
    o << std::endl << "body_size: " << c.body_size();
    o << std::endl << "methods: ";
    cforeach(std::set<std::string>, c.methods(), it)
        o << *it << ' ';
    o << std::endl;
    return o;
}
