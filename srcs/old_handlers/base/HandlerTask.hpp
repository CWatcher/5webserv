
#ifndef HANDLERTASK_HPP
# define HANDLERTASK_HPP

# include "config/ServerConfig.hpp"
# include "socket/SessionSocket.hpp"

struct HandlerTask
{
    HandlerTask(const Location &loc, SessionSocket *sess)
        : location(loc), session(sess) {}

    const Location  &location;
    SessionSocket   *session;
};

#endif
