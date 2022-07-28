#include "config/ServerConfig.hpp"
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

    while (true)
    {
        try
        {
            ServerConfig config(argv[1]);
            std::clog << config;

            Server server(config);

            try
            {
                server.mainLoopRun();
            }
            catch (std::exception &e)
            {
                logger::error << "main: unhandled exception: " << e.what() << "errno: " << logger::cerror << logger::end;
                // TODO: clean all allocated by server and send 500 to all sessions (implement in destructors!)
                sleep(5);
            }
        }
        catch (const ServerConfig::bad_config &e)
        {
            logger::error << e.what() << logger::end;
            return EXIT_FAILURE;
        }
        catch (std::exception &e)
        {
            logger::error << logger::cerror << logger::end;
            return EXIT_FAILURE;
        }
    }
}
