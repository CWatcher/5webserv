#include "Parser.hpp"

Parser::Parser(const char* filename)
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
}

std::ostream&   operator<<(std::ostream& o, const Parser& p)
{
    for (size_t i = 0; i < p.getServers().size(); i++)
    {
        o << std::string(10, '-') << "Server " << i << std::string(10, '-') << std::endl;
        o << p.getServers()[i];
    }
    return o;
}
