
#ifndef HANDLERTASK_HPP
# define HANDLERTASK_HPP

# include "config/ServerConfig.hpp"
# include "socket/SocketSession.hpp"

struct HandlerTask
{
    HandlerTask(const Location &loc, SocketSession *sess)
        : location(loc), session(sess) {}

    const Location  &location;
    SocketSession   *session;
};

#endif
