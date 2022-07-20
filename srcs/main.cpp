#include "utils/log.hpp"
#include "utils/syntax.hpp"
#include "config/Config.hpp"
#include <arpa/inet.h>

#include <iostream>
using std::cout;
using std::endl;

int main(int, char* argv[])
{
    try
    {
        Config  config(argv[1]);

        std::clog << config << std::endl;
        std::clog << config.getServer(inet_addr("0.0.0.0"), ntohs(80), "zzz");
    }
    // ошибки, которые я обнаружил при парсинге (не удалось открыть файл, некорректные значения в файле ...)
    catch(const Config::bad_config& e)
    {
        logger::error << e.what() << logger::end;
    }
    // cистемные ошибки при работе с файлом
    catch(const std::ifstream::failure &e)
    {
        logger::error << logger::cerror << logger::end;
    }
    // все остальные системные ошибки (bad_alloc?), можно совместить с std::ifstream::failure
    catch(const std::exception& e)
    {
        logger::error << logger::cerror <<logger::end;
    }
    return 0;
}
