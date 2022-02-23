#include "ASocket.hpp"
#include "SocketListen.hpp"
#include "SocketSession.hpp"
#include "Logger.hpp"

#include <sys/poll.h>
#include <netinet/in.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include <map>

const in_port_t	ListenPort = 9999;
const int		ConnectionsLimit = 4;
const int		PollTimeout = 30 * 1000;


const char   *DefaultResponse =	"HTTP/1.1 200 OK\n"
                                  "Date: Wed, 18 Feb 2021 11:20:59 GMT\n"
                                  "Server: Apache\n"
                                  "X-Powered-By: webserv\n"
                                  "Last-Modified: Wed, 11 Feb 2009 11:20:59 GMT\n"
                                  "Content-Type: text/html; charset=utf-8\n"
                                  "Content-Length: 14\n"
                                  "Connection: close\n"
                                  "\n"
                                  "<h1>HELLO WORLD</h1>\n";

void	socketManagerRun(std::map<int, ASocket *> sockets);

int main()
{
	ASocket						*socket_listen;
	std::map<int, ASocket *>	sockets_array;

	log::setLevel(Logger::Debug);

	try
	{
		socket_listen = new SocketListen(ListenPort, ConnectionsLimit);
	}
	catch (std::exception &e)
	{
		log::cerrno();
		return EXIT_FAILURE;
	}
	sockets_array[socket_listen->fd] = socket_listen;

	log::info("Entering main loop...");
	socketManagerRun(sockets_array);
}

void	socketManagerRun(std::map<int, ASocket *> sockets)
{
	std::vector<pollfd>			pollfds;
	size_t						pollfds_len;
	int							new_events;
	enum ASocket::PostAction	post_action;
	int							action_value;

	while (true)
	{
		try
		{
			pollfds.reserve(sockets.size());
		}
		catch (std::bad_alloc &e)
		{
			log::error(e.what());
		}

		pollfds_len = 0;
		for (std::map<int, ASocket *>::const_iterator it = sockets.begin(); it != sockets.end(); ++it)
		{
			ASocket	*socket = it->second;

			if (pollfds_len >= pollfds.capacity())
			{
				log::warning("Not enough memory to add socket", socket->fd);
				sockets.erase(socket->fd);
				delete socket;
				continue ;
			}
			pollfds[pollfds_len].fd = socket->fd;
			if (socket->getTrigger() == TriggerEvent::Read)
				pollfds[pollfds_len].events = POLLIN;
			else if (socket->getTrigger() == TriggerEvent::Write)
				pollfds[pollfds_len].events = POLLOUT;

			++pollfds_len;
		}

		new_events = poll(&pollfds[0], pollfds_len, PollTimeout);

		if (new_events <= 0)
		{
            if (errno)
                log::cerrno();
            else
                log::debug("No new events. Reached poll timeout (seconds):", PollTimeout / 1000);
			continue ;
        }

		for (size_t i = 0; i < pollfds_len; ++i)
		{
			pollfd	*poll_fd = &pollfds[i];

			if (poll_fd->revents == 0)
				continue;

			ASocket	*socket = sockets[poll_fd->fd];

			if (poll_fd->revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				log::info("Got terminating event on socket", poll_fd->fd);
				sockets.erase(poll_fd->fd);
				delete socket;
			}
			else if (poll_fd->revents == poll_fd->events)
			{
				action_value = socket->action(post_action);

				if (action_value == -1)
					log::cerrno();

                HTTPMessage DUMMY_RESPONSE; // TODO: delete after response implementation
                DUMMY_RESPONSE.raw_data = DefaultResponse;

				switch (post_action)
				{
					case ASocket::Add:
						sockets[action_value] = new SocketSession(action_value);
						break ;

					case ASocket::Process:
						log::info("Sent to process queue socket", poll_fd->fd);
						// put to real Process queue later

						log::info("HTTP response is ready for socket", poll_fd->fd);
                        dynamic_cast<SocketSession *>(socket)->prepareForWrite(DUMMY_RESPONSE);
						break ;

					case ASocket::Disconnect:
						sockets.erase(socket->fd);
						delete socket;
						break ;

					default:
						break ;
				}
			}
		}
	}
}
