// #include "utils/log.hpp"
#include "Parser.hpp"
// #include "parser_utils.hpp"

// #include <fstream>
// #include <exception>
// #include <cstring>
// #include <cerrno>
//
#include <iostream>
// #include <sstream>
// #include <arpa/inet.h>
// #include <netinet/in.h>
using std::cout;
using std::endl;

int main(int, char* argv[])
{
    Parser  p(argv[1]);
    cout << p.root() << endl;
    cout << p;
    return 0;
}
