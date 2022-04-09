#include "log.hpp"

int main()
{
    logger::setLevel("info");
    logger::debug("Message") << "\taddtion data: " << 1 << '@' << " go on\n";
    logger::info("Message") << "\taddtion data: " << 1 << '@' << " go on\n";
}