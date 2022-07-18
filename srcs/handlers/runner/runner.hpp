
#ifndef RUNNER_HPP
# define RUNNER_HPP

# include "../base/IHandler.hpp"

namespace handlers
{
    extern IHandler *start_handler;

    void *run(void *data);
}

#endif
