
#include "SocketListen.hpp"

#include <arpa/inet.h>
#include <sstream>

SocketListen::SocketListen(in_port_t port, int connections_limit)
	: ASocket(socket(AF_INET, SOCK_STREAM, IPPROTO_IP), TriggerEvent::Read)
{
	const int	sockopt_value = 1;
	sockaddr_in	socket_address;

	if (fd == -1)
		throw std::exception();

	log::debug("Created listen socket", fd);

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
				   &sockopt_value, sizeof(sockopt_value)))
		throw std::exception();

	log::debug("Enabled SO_REUSEADDR for socket", fd);

	socket_address.sin_family = AF_INET;
	socket_address.sin_addr.s_addr = inet_addr("0.0.0.0");
	socket_address.sin_port = htons(port);
	if (bind(fd, (sockaddr *)&socket_address, sizeof(socket_address)))
		throw std::exception();

	log::debug("Bound socket to network interface with port", port);

	if (listen(fd, connections_limit))
		throw std::exception();

	log::debug("Enabled listen on port", port);
}

int	SocketListen::action(enum PostAction &post_action)
{
	sockaddr_in	client_address;
	socklen_t	address_len = sizeof(client_address);
	int			new_socket;

	log::debug("Trying to accept socket", fd);
	new_socket = accept(fd, (sockaddr *)&client_address, &address_len);

	if (new_socket == -1)
		post_action = NoAction;
	else
	{
		std::stringstream	logstream;
		logstream << "Connected new client on socket " << new_socket
				  << " (" << inet_ntoa(client_address.sin_addr)
				  << ":" << ntohs(client_address.sin_port) << ")";
		log::info(logstream.str());

		post_action = Add;
	}

	return new_socket;
}
