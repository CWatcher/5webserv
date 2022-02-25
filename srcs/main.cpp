#include "Logger.hpp"
#include "ASocket.hpp"
#include "SocketListen.hpp"
#include "Server.hpp"

#include <netinet/in.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <map>

const in_port_t	ListenPort = 9999;
const int		ConnectionsLimit = 4;

int main()
{
	ASocket						*socket_listen;
	std::map<int, ASocket *>	sockets_array;

	logger::setLevel(Logger::Debug);

	try{
		socket_listen = new SocketListen(ListenPort, ConnectionsLimit);
	}
	catch (std::exception &e){
		logger::cerrno();
		return EXIT_FAILURE;
	}

	sockets_array[socket_listen->fd] = socket_listen;

    Server server(sockets_array);

    server.mainLoopRun();
}
