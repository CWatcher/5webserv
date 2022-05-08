#include "utils/log.hpp"
#include "utils/syntax.hpp"
#include "config/Config.hpp"

#include <iostream>
using std::cout;
using std::endl;

int main(int, char* argv[])
{
    try
    {
        // этот блок нужно будет поместить в условный initWebserver()
        Config              config(argv[1]);
        std::set<Listen>    listened;
        // config.getServers() возвращает вектор всех серверов в конфиге, нужно будет его сохранить
        // в listened будут уникальные пары host:port в формате который можно отдавать в socket и bind
        cforeach(std::vector<Server>, config.getServers(), server)
            listened.insert(server->listen());
        cout << config;
        // oбъект config больше не нужен
    }
    //ошибки, которые я обнаружил при парсинге (неизвестная опция, слишком много значений для опции ...)
    catch(const std::logic_error& e)
    {
        logger::error << "configuration file error: " << e.what() << logger::end;
    }
    // все остальные системные ошибки (не удалось открыть файл ...), в том числе неожиданный конец файла
    catch(const std::exception& e)
    {
        logger::error << "configuration file error: " << logger::cerror <<logger::end;
    }
    return 0;
}
