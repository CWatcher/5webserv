#include "log.hpp"
#include "math.h"

int main()
{
    logger::setLevel("info");
    logger::debug << "addtion data: " << 1 << '@' << " go on" << logger::end;
    logger::info << "addtion data: " << logger::cerror << '@' << " go on" << logger::end;
}
