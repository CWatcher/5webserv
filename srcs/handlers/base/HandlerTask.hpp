
#ifndef HANDLERTASK_HPP
# define HANDLERTASK_HPP

# include "config/ServerConfig.hpp"
# include "SocketSession.hpp"

struct HandlerTask
{
    HandlerTask(const ServerConfig &config, SocketSession *session)
        : config(config), session(session) {}

    const ServerConfig config;
    SocketSession      *session;
};

#endif
