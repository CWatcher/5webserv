#include <stdlib.h> // EXIT_FAILURE
#include "utils/log.hpp"
#include "config/ServerConfig.hpp"
#include "Server.hpp"

int main(int, char* argv[])
{
    logger::setLevel(logger::Level::kDebug);

    try
    {
        Server  server(argv[1]);

        server.mainLoopRun();
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
