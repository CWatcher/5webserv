#include "utils/log.hpp"
#include "utils/syntax.hpp"
#include "config/ConfigParser.hpp"

#include <iostream>
using std::cout;
using std::endl;

int main(int, char* argv[])
{
    try
    {
        ConfigParser  config(argv[1]);
        config.parseConfig();
        // cforeach(std::vector<Server>, config.getServers(), server)
        //     listened.insert(server->listen());
        std::clog << config;
    }
    //ошибки, которые я обнаружил при парсинге (неизвестная опция, слишком много значений для опции ...)
    catch(const std::logic_error& e)
    {
        logger::error << e.what() << logger::end;
    }
    // все остальные системные ошибки (не удалось открыть файл ...), в том числе неожиданный конец файла
    catch(const std::exception& e)
    {
        logger::error << logger::cerror <<logger::end;
    }
    return 0;
}
