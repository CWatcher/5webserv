#include "utils/log.hpp"
#include "Server.hpp"

#include <fstream>
#include <exception>
#include <cstring>
#include <cerrno>
// 
#include <iostream>
#include <sstream>
using std::cout;
using std::endl;
int main(int, char* argv[])
{
    // logger::setLevel("debug");

    if (argv[1])
    {
        std::ifstream       file(argv[1]);
        std::string         token;
        std::stringstream   ss;

        if (file.fail())
        {
            logger::error << logger::cerror << logger::end;
            throw std::runtime_error(strerror(errno));
        }
        else
            logger::info << "configuration file opened \"" << argv[1] << "\"" << logger::end;
        
        std::getline(file >> std::ws, token);
        cout << "|" << token << "|" << endl;
        std::getline(file >> std::ws, token);
        cout << "|" << token << "|" << endl;
        // while (file >> token)
        // {
        //     if (token == "server")
        //     {
        //         Server  server;
        //         server.readConfig(file);
        //     }
        // }
    }

    return 0;
}
