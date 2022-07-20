#include "config/Config.hpp"
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

    Config config(argv[1]);
    try
    {
//        Config config(argv[1]);
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

    std::clog << config;

    while (true)
    {
        try
        {
            Server server(config);
            server.mainLoopRun();
        }
        catch (std::exception &e)
        {
            logger::error << "main: unhandled exception: " << e.what() << "errno: " << logger::cerror << logger::end;
            // TODO: clean all allocated by server (implement in destructors!)
            sleep(5);
        }
    }
}
