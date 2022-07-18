#include "config/ConfigParser.hpp"
#include "utils/log.hpp"
#include "utils/syntax.hpp"
#include "ASocket.hpp"
#include "SocketListen.hpp"
#include "Server.hpp"

#include <netinet/in.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <map>
#include <csignal>
#include <unistd.h>
#include <cerrno>
#include <cstring>

const in_port_t	ListenPort = 9999;
const int		ConnectionsLimit = 4;

//void signal_handler(int _)
//{
//    (void)_;
//    logger::debug("Catched signal");
//}

int main(int, char* argv[])
{
    try
    {
        ConfigParser  config(argv[1]);
        config.parse();
        config.getServers();
        std::clog << config;
    }
        //ошибки, которые я обнаружил при парсинге (неизвестная опция, слишком много значений для опции ...)
    catch(const std::logic_error& e)
    {
        logger::error << e.what() << logger::end;
    }
        // все остальные системные ошибки (не удалось открыть файл, ошибка при чтении файла ...)
    catch(const std::exception& e)
    {
        logger::error << logger::cerror <<logger::end;
    }


	ASocket						*socket_listen;
	std::map<int, ASocket *>	sockets_array;

//    signal(SIGUSR1, signal_handler);
	logger::setLevel("debug");

	try{
		socket_listen = new SocketListen(ListenPort, ConnectionsLimit);
	}
	catch (std::exception &e){
        logger::error << __FUNCTION__ << ": " << strerror(errno) << logger::end;
		return EXIT_FAILURE;
	}

	sockets_array[socket_listen->fd] = socket_listen;

    Server server(sockets_array);
    server.mainLoopRun();
}
