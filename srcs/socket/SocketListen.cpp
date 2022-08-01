
#include "socket/SocketListen.hpp"
#include "utils/log.hpp"

#include <arpa/inet.h>

int SocketListen::ConnectionsLimit = 1024;

SocketListen::SocketListen(in_addr_t ip, in_port_t port, int connections_limit)
    : ASocket(socket(AF_INET, SOCK_STREAM, IPPROTO_IP), ip, port)
{
    const int	sockopt_value = 1;
    sockaddr_in	socket_address;
    char addr_str[40];
    struct in_addr addr;

    addr.s_addr = ip;
    inet_ntop(AF_INET, &addr, addr_str, 40);
    if (_fd == -1)
    {
        logger::error << "Cann't create listen socket on " << addr_str << ':' << ntohs(port) << logger::end;
        throw std::exception();
    }

    logger::debug << "Created listen socket " << _fd << logger::end;

    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR,
                   &sockopt_value, sizeof(sockopt_value)))
        throw std::exception();

    logger::debug << "Enabled SO_REUSEADDR for socket " << _fd << logger::end;

    logger::debug << "Trying to bind to network interface: " << addr_str << ':' << ntohs(port) << logger::end;

    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = ip;
    socket_address.sin_port = port;
    if (bind(_fd, (sockaddr *)&socket_address, sizeof(socket_address)))
        throw std::exception();

    if (listen(_fd, connections_limit))
        throw std::exception();

    logger::info << "Enabled listen on: " << addr_str << ':' << ntohs(port) << logger::end;
}

int SocketListen::action()
{
    sockaddr_in	client_address;
    socklen_t	address_len = sizeof(client_address);
    int			new_fd;

    logger::debug << "Accepting on port: " << ntohs(_port) << " (socket " << _fd << ")" << logger::end;
    new_fd = accept(_fd, (sockaddr *)&client_address, &address_len);

    if (new_fd == -1)
        logger::error << "Accept error on port " << ntohs(_port) << " (socket " << _fd << ")" << logger::end;
    else
        logger::info << "Connected new client: socket " << new_fd
            << " (" << inet_ntoa(client_address.sin_addr)
            << ":" << ntohs(client_address.sin_port) << ")" << logger::end;
    return new_fd;
}
