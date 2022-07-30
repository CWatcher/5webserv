
#ifndef SERVER_HPP
# define SERVER_HPP

# include "utils/thread_pool.hpp"
# include "config/ServerConfig.hpp"

# include <map>
# include <sys/poll.h>
# include <vector>
# include <netinet/in.h>

class ASocket;
class SocketSession;

class Server
{
public:
    explicit Server(ServerConfig &config);
    ~Server();

    void    mainLoopRun();
    void    deleteSession(int fd);
    void    addProcessTask(SocketSession *session);
    void    addSession(int fd, in_addr_t from_listen_ip, in_port_t from_listen_port);

    static int poll_timeout;

private:
    Server();
    Server(const Server &_);
    Server &operator=(const Server &_);

    size_t  eventArrayPrepare(std::vector<pollfd> &poll_array) const;
    bool    eventCheck(const pollfd *poll_fd);

    ServerConfig             &_config;
    std::map<int, ASocket *> _sockets;
    ft::thread_pool          _thread_pool;
};

#endif
