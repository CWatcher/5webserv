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
    //ошибки, которые я обнаружил при парсинге (неизвестная опция, слишком много значений для опции, не удалось открыть файл ...)
    catch(const std::logic_error& e)
    {
        logger::error << e.what() << logger::end;
    }
    catch(const std::runtime_error& e)
    {
        logger::error << e.what() << logger::end;
    }
    // все остальные системные ошибки (ошибка при чтении файла ...)
    catch(const std::exception& e)
    {
        logger::error << logger::cerror <<logger::end;
    }
    return 0;
}
