
#ifndef HANDLERTASK_HPP
# define HANDLERTASK_HPP

# include "config/ServerConfig.hpp"
# include "SocketSession.hpp"

struct HandlerTask
{
    HandlerTask(const Location &config, SocketSession *session)
        : config(config), session(session) {}

    const Location  config;
    SocketSession   *session;
};

#endif
