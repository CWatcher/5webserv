
#ifndef RUNNER_HPP
# define RUNNER_HPP

# include "../base/IHandler.hpp"

namespace handlers
{
    extern IHandler *runner;

    void *run(void *data);
}

#endif
