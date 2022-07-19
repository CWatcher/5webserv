
#ifndef SERVER_HPP
# define SERVER_HPP

# include "ASocket.hpp"
# include "utils/thread_pool.hpp"
#include "config/ServerConfig.hpp"

# include <map>
# include <sys/poll.h>
# include <vector>
#include <netinet/in.h>

class Server
{
public:
    explicit Server(const std::vector<ServerConfig> &configs);
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
    void    addProcessTask(ASocket *socket);

    std::map<int, ASocket *> _sockets;
    std::map<std::pair<in_addr_t, in_port_t>, std::map<std::string, ServerConfig> > _config_by_address;
    ft::thread_pool          _thread_pool;
};

#endif
