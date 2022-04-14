#include "log.hpp"

int main()
{
    logger::setLevel("info");
    logger::debug << "addtion data: " << 1 << '@' << " go on\n";
    logger::info << "addtion data: " << 1 << '@' << " go on\n";
}