#include "log.hpp"

int main()
{
    logger::setLevel("info");
    logger::debug << "addition data: " << 1 << '@' << " go on" << logger::end;
    logger::info << "addition data: " << logger::cerror << '@' << " go on" << logger::end;
}
