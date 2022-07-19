#include "config/ConfigParser.hpp"
#include "utils/log.hpp"
#include "Server.hpp"

#include <cstdlib>
#include <iostream>
#include <vector>
#include <cerrno>
#include <cstring>
#include <csignal>
#include <unistd.h>

//void signal_handler(int _)
//{
//    (void)_;
//    logger::debug("Catched signal");
//}

int main(int, char* argv[])
{
//    signal(SIGUSR1, signal_handler);
    logger::setLevel(logger::Level::kDebug);

    std::vector<ServerConfig> server_configs;
    try
    {
        ConfigParser config(argv[1]);
        config.parse();
        server_configs = config.getServers();
        std::clog << config;
    }
    catch (const std::logic_error &e)
    {
        logger::error << "ConfigParser: " << e.what() << logger::end;
        return EXIT_FAILURE;
    }
    catch (const std::exception &e)
    {
        logger::error << "ConfigParser: " << logger::cerror << logger::end;
        return EXIT_FAILURE;
    }

    try
    {
        Server server(server_configs);
        server.mainLoopRun();
    }
    catch (std::exception &e){
        logger::error << "main: caught unhandled exception: " << strerror(errno) << logger::end;
        return EXIT_FAILURE;
    }
}
