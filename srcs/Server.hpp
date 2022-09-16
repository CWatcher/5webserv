#ifndef SERVER_HPP
# define SERVER_HPP

# include "config/ServerConfig.hpp"
# include "sockets/ASocket.hpp"
# include "sockets/SessionSocket.hpp"
# include "handlers/AHandler.hpp"

# include <map>
# include <vector>
# include <sys/poll.h>

class Server
{
public:
    explicit Server(const char *filename);
    ~Server();

    void        mainLoopRun();

private:
    size_t      eventArrayPrepare(std::vector<pollfd> &poll_array) const;
    bool        eventCheck(const pollfd *poll_fd);
    void        eventAction(ASocket *socket);
    void        handleRequest(ASocket *session);
    AHandler*   getHandler(const Location &location, SessionSocket *session) const;

private:
    ServerConfig             _config;
    std::map<int, ASocket *> _sockets;
};

#endif
