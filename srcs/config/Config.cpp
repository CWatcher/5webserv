#include "Config.hpp"
#include "utils/log.hpp"

Config::Config(const char* filename)
{
    std::ifstream   f;
    std::string     str;

    if (filename == NULL)
        filename = "config";
    f.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    f.open(filename);
    while (!(f >> std::ws).eof())
    {
        if (f.peek() == '#')
        {
            std::getline(f, str);
            continue;
        }
        f >> str;
        f.exceptions(std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit);
        if (str == "root")
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
        else if (str == "server")
            servers_.push_back(Server(*this, f));
        else
            throw std::logic_error("unknown directive \"" + str + "\"");
        f.exceptions(std::ifstream::goodbit);
    }
    f.close();
    logger::info << "configuration file \"" << filename << "\" loaded" << logger::end;
}

std::ostream&   operator<<(std::ostream& o, const Config& p)
{
    for (size_t i = 0; i < p.getServers().size(); i++)
    {
        o << "\033[0;32m" << std::string(10, '-') << "Server " << i << std::string(10, '-') << "\033[0m" << std::endl;
        o << p.getServers()[i];
    }
    return o;
}
