#include "utils/log.hpp"
#include "utils/syntax.hpp"
#include "config/Config.hpp"
#include <arpa/inet.h>

#include <iostream>
using std::cout;
using std::endl;


// const Location&   getLocation(const Location& server, const std::string& uri)
// {
//     const Location* location = &server;
//     std::string     tmp_uri = uri;

//     while (!tmp_uri.empty() && !location->locations.empty())
//     {
//         std::map<std::string, Location>::const_iterator next_location = location->locations.find(tmp_uri + "/");
//         if (next_location != location->locations.end())
//         {
//             tmp_uri = uri;
//             location = &next_location->second;
//             continue;
//         }
//         tmp_uri.erase(tmp_uri.rfind('/'));
//     }
//     return *location;
// }


int main(int, char* argv[])
{
    try
    {
        Config  config(argv[1]);

        std::clog << config << std::endl;
        // const VirtualServer& server = config.getServer(inet_addr("0.0.0.0"), ntohs(80), "nested");
        // std::clog << server;
        // std::string uri;
        // while (std::getline(std::cin, uri))
        // {
        //     const BaseConfig& location = getLocation(server, uri);
        //     cout << "location: " << location.path << endl;
        // }

    }
    // ошибки, которые я обнаружил при парсинге (не удалось открыть файл, некорректные значения в файле ...)
    catch(const Config::bad_config& e)
    {
        logger::error << e.what() << logger::end;
    }
    // cистемные ошибки при работе с файлом
    catch(const std::ifstream::failure &e)
    {
        logger::error << logger::cerror << logger::end;
    }
    // все остальные системные ошибки (bad_alloc?), можно совместить с std::ifstream::failure
    catch(const std::exception& e)
    {
        logger::error << logger::cerror <<logger::end;
    }
    return 0;
}
