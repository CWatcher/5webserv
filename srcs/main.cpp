#include "utils/log.hpp"
#include "utils/syntax.hpp"
#include "config/ServerConfig.hpp"
#include <arpa/inet.h>

#include <iostream>
using std::cout;
using std::endl;


int main(int, char* argv[])
{
    try
    {
        ServerConfig  config(argv[1]);

        std::clog << config << std::endl;
        const VirtualServer& server = config.getVirtualServer(inet_addr("0.0.0.0"), ntohs(80), "nested");
        std::clog << server;
        std::string uri;
        while (std::getline(std::cin, uri))
        {
            const Location& location = VirtualServer::getLocation(server, uri);
            cout << "location: " << location.path << endl;
        }
    }
    // ошибки, которые я обнаружил при парсинге (не удалось открыть файл, некорректные значения в файле ...)
    catch(const ServerConfig::bad_config& e)
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
