#include "utils/log.hpp"
#include "Webserver.hpp"
#include "Server.hpp"
#include "parser_utils.hpp"

#include <fstream>
#include <exception>
#include <cstring>
#include <cerrno>
// 
#include <iostream>
#include <sstream>
#include <arpa/inet.h>
#include <netinet/in.h>
using std::cout;
using std::endl;

int main(int, char* argv[])
{
    Webserver webserver;
    
    if (argv[1])
        webserver.parseConfig(argv[1]);

    return 0;
}
