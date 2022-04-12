#include "log.hpp"

int main()
{
    logger::debug() << "addtion data: " << 1 << '@' << " go on";
    errno = EINVAL;
    logger::puterrno();
    logger::setLevel("info");
    logger::info() << "Entering main Server loop...";
    logger::warning() << "Connected new client on socket " << 4 << " addr: 127.0.0.1:" << 43562;



    logger::error() << "Read from socket (bytes):" << 717;

    logger::error("") << "\tRead from socket (bytes):" << 717 << '\n';

}