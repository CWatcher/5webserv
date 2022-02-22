#include "ASocket.hpp"
#include "SocketListen.hpp"
#include "SocketRead.hpp"
#include "SocketWrite.hpp"
#include "Logging.hpp"

#include <sys/poll.h>
#include <netinet/in.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include <map>

const in_port_t	ListenPort = 9999;
const int		ConnectionsLimit = 4;
const int		PollTimeout = 30 * 1000;

void	socketManagerRun(std::map<int, ASocket *> sockets_array);

int main()
{
	ASocket						*socket_listen;
	std::map<int, ASocket *>	sockets_array;

	log::setLevel("debug");

	try
	{
		socket_listen = new SocketListen(ListenPort, ConnectionsLimit);
	}
	catch (std::exception &e)
	{
		log::errno_error();
		return EXIT_FAILURE;
	}
	sockets_array[socket_listen->fd] = socket_listen;

	log::info("Entering main loop...");
	socketManagerRun(sockets_array);
}

void	socketManagerRun(std::map<int, ASocket *> sockets_array)
{
	std::vector<pollfd>			pollfd_array;
	size_t						pollfd_array_len;
	int							new_events;
	enum ASocket::PostAction	post_action;
	int							action_value;

	while (true)
	{
		try
		{
			pollfd_array.reserve(sockets_array.size());
		}
		catch (std::bad_alloc &e)
		{
			log::error(e.what());
		}

		pollfd_array_len = 0;
		for (std::map<int, class ASocket *>::const_iterator it = sockets_array.begin(); it != sockets_array.end(); ++it)
		{
			ASocket	*socket = it->second;

			if (pollfd_array_len >= pollfd_array.capacity())
			{
				log::warning("Not enough memory to add socket", socket->fd);
				socket->disconnect();
				sockets_array.erase(socket->fd);
				delete socket;
				continue ;
			}
			pollfd_array[pollfd_array_len].fd = socket->fd;
			if (socket->trigger == TriggerType::Read)
				pollfd_array[pollfd_array_len].events = POLLIN;
			else if (socket->trigger == TriggerType::Write)
				pollfd_array[pollfd_array_len].events = POLLOUT;

			++pollfd_array_len;
		}

		new_events = poll(&pollfd_array[0], pollfd_array_len, PollTimeout);

		if (new_events == -1)
		{
			log::errno_error();
			continue ;
		}
		else if (new_events == 0)
		{
			log::debug("No new events. Reached poll timeout (seconds):", PollTimeout / 1000);
			continue;
		}

		for (size_t i = 0; i < pollfd_array_len; ++i)
		{
			pollfd	*poll_fd = &pollfd_array[i];

			if (poll_fd->revents == 0)
				continue;

			ASocket	*socket = sockets_array[poll_fd->fd];

			if (poll_fd->revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				log::info("Got terminating event on socket", poll_fd->fd);
				socket->disconnect();
				sockets_array.erase(poll_fd->fd);
				delete socket;
			}
			else if (poll_fd->revents == poll_fd->events)
			{
				action_value = socket->action(post_action);

				if (action_value == -1)
					log::errno_error();

				switch (post_action)
				{
					case ASocket::Add:
						sockets_array[action_value] = new SocketRead(action_value);
						break ;

					case ASocket::Process:
						log::info("Sent to process queue socket", poll_fd->fd);
						// put to real Process queue later

						log::info("HTTP response is ready for socket", poll_fd->fd);
						sockets_array.erase(socket->fd);
						sockets_array[socket->fd] = new SocketWrite(*socket);
						delete socket;
						break ;

					case ASocket::Read:
						sockets_array.erase(socket->fd);
						sockets_array[socket->fd] = new SocketRead(*socket);
						delete socket;
						break ;

					case ASocket::Disconnect:
						socket->disconnect();
						sockets_array.erase(socket->fd);
						delete socket;
						break ;

					default:
						break ;
				}
			}
		}
	}
}
