
#include "SocketListen.hpp"

#include <arpa/inet.h>
#include <sstream>

SocketListen::SocketListen(in_addr_t ip, in_port_t port, int connections_limit)
	: ASocket(socket(AF_INET, SOCK_STREAM, IPPROTO_IP), TriggerType::Read), ip(ip), port(port)
{
	const int	sockopt_value = 1;
	sockaddr_in	socket_address;

	if (fd == -1)
		throw std::exception();

	logger::debug << "Created listen socket " << fd << logger::end;

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
				   &sockopt_value, sizeof(sockopt_value)))
		throw std::exception();

    logger::debug << "Enabled SO_REUSEADDR for socket " << fd << logger::end;

    char addr_str[40];
    struct in_addr addr;
    addr.s_addr = ip;
    inet_ntop(AF_INET, &addr, addr_str, sizeof(addr_str));
    logger::debug << "Trying to bind to network interface: " << addr_str << ':' << ntohs(port) << logger::end;

    socket_address.sin_family = AF_INET;
	socket_address.sin_addr.s_addr = ip;
	socket_address.sin_port = port;
	if (bind(fd, (sockaddr *)&socket_address, sizeof(socket_address)))
		throw std::exception();

	if (listen(fd, connections_limit))
		throw std::exception();

    logger::debug << "Enabled listen on: " << addr_str << ':' << ntohs(port) << logger::end;
}

int	SocketListen::action(enum PostAction &post_action)
{
	sockaddr_in	client_address;
	socklen_t	address_len = sizeof(client_address);
	int			new_socket;

    logger::debug << "Accepting on port: " << ntohs(port) << " (socket " << fd << ")" << logger::end;
	new_socket = accept(fd, (sockaddr *)&client_address, &address_len);

	if (new_socket == -1)
		post_action = NoAction;
	else
	{
        logger::info << "Connected new client: socket " << new_socket
				  << " (" << inet_ntoa(client_address.sin_addr)
				  << ":" << ntohs(client_address.sin_port) << ")" << logger::end;
		post_action = Add;
	}

	return new_socket;
}
