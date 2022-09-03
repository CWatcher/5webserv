
#ifndef SERVER_HPP
# define SERVER_HPP

# include "utils/thread_pool.hpp"
# include "config/ServerConfig.hpp"
# include "sockets/ASocket.hpp"
# include "utils/log.hpp"

# include <map>
# include <sys/poll.h>
# include <vector>
# include <netinet/in.h>

class Server
{
public:
    explicit Server(const char *filename);
    ~Server();

    void    mainLoopRun();

    static int poll_timeout;

private:
    Server();
    Server(const Server &_);
    Server &operator=(const Server &_);

    size_t  eventArrayPrepare(std::vector<pollfd> &poll_array) const;
    bool    eventCheck(const pollfd *poll_fd);
    void    eventAction(ASocket *socket);
    void    addProcessTask(ASocket *session);

    ServerConfig             _config;
    std::map<int, ASocket *> _sockets;
    ft::thread_pool          _thread_pool;
};

#endif
