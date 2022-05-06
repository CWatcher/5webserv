#include "Parser.hpp"

#include <iostream>
Parser::Parser(const char* filename)
    // parser_(parser_init_list_, parser_init_list_ + 5)
{
    std::ifstream   f;
    std::string     str;

    if (filename == NULL)
        filename = "config";
    // f.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    f.open(filename);
    while (true)
    {
        if ((f >> std::ws).peek() == '#')
        {
            std::cout << "skip" << std::endl;
            std::getline(f, str);
            continue;
        }
        f >> str;
        if (f.eof())
            break;
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
            throw std::logic_error("undefined directive \"" + str + "\"");
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

// void    Parser::addServer(std::ifstream& f)
// {
//     Server  server(*this, f);

//     // listen_.insert(server.listen());
//     servers_.push_back(server);
// }
