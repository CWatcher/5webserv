
#ifndef THREADPOOL_HPP
# define THREADPOOL_HPP

# include "SocketSession.hpp"

# include <queue>
# include <vector>


class ThreadPool
{
public:
    ThreadPool();
    ~ThreadPool();

    void            pushTaskToQueue(SocketSession *task);
    SocketSession   *popTaskFromQueue();

private:
    void        threadsStart(long int threads_count);
    static void *threadLoop(void *thread_pool_void);


    bool                        _is_running;
    std::vector<pthread_t>      _threads;
    std::queue<SocketSession *> _tasks;
    pthread_mutex_t             _tasks_lock;
    pthread_cond_t              _tasks_event;
};

#endif
