
#include "sockets/ListenSocket.hpp"
#include "utils/log.hpp"

#include <arpa/inet.h>

int ListenSocket::ConnectionsLimit = 1024;

ListenSocket::ListenSocket(in_addr_t ip, in_port_t port, int connections_limit)
    : ASocket(socket(AF_INET, SOCK_STREAM, IPPROTO_IP), ip, port)
{
    const int   sockopt_value = 1;
    sockaddr_in socket_address;
    in_addr     addr;
    char        addr_str[40];

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

int ListenSocket::action(in_addr &remote_addr)
{
    sockaddr_in client_addr;
    socklen_t	client_addr_len = sizeof(client_addr);
    int			new_fd;

    logger::debug << "Accepting on port: " << ntohs(_port) << " (socket " << _fd << ")" << logger::end;
    new_fd = accept(_fd, (sockaddr *)&client_addr, &client_addr_len);

    if (new_fd == -1)
        logger::error << "Accept error on port " << ntohs(_port) << " (socket " << _fd << ")" << logger::end;
    else
        logger::info << "Connected new client: socket " << new_fd
            << " (" << inet_ntoa(client_addr.sin_addr)
            << ":" << ntohs(client_addr.sin_port) << ")" << logger::end;
    remote_addr = client_addr.sin_addr;
    return new_fd;
}
