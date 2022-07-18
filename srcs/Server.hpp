
#ifndef SERVER_HPP
# define SERVER_HPP

# include "ASocket.hpp"
# include "utils/thread_pool.hpp"

# include <map>
# include <sys/poll.h>
# include <vector>

class Server
{
public:
//    Server(server configs); TODO: switch to configs for constructor
    explicit Server(const std::map<int, ASocket *> &sockets_to_listen);
    ~Server();

    void       mainLoopRun();

    static int poll_timeout;

private:
    Server();
    Server(const Server &_);
    Server &operator=(const Server &_);

    size_t  eventArrayPrepare(std::vector<pollfd> &poll_array);
    bool    eventCheck(const pollfd *poll_fd);
    void    eventAction(ASocket *socket);

    std::map<int, ASocket *> _sockets;
    ft::thread_pool          _thread_pool;
};

#endif
