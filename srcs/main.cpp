#include "utils/log.hpp"
#include "Server.hpp"

#include <cstdlib>

int main(int, char* argv[])
{
    logger::setLevel(logger::Level::kDebug);

    try
    {
        Server  server(argv[1]);

        server.mainLoopRun();
    }
    catch (const std::runtime_error &re)
    {
        logger::error << re.what() << logger::end;
        return EXIT_FAILURE;
    }
    catch (const std::exception&)
    {
        logger::error << logger::cerror << logger::end;
        return EXIT_FAILURE;
    }
}
