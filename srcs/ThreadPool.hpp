
#ifndef THREADPOOL_HPP
# define THREADPOOL_HPP

# include "HTTPMessage.hpp"

# include <pthread.h>
# include <unistd.h>
# include <algorithm>
# include <cerrno>
# include <queue>
# include <utility>
# include <vector>

const char      *DefaultResponse =	  "HTTP/1.1 200 OK\n"
                                        "Date: Wed, 18 Feb 2021 11:20:59 GMT\n"
                                        "Server: Apache\n"
                                        "X-Powered-By: webserv\n"
                                        "Last-Modified: Wed, 11 Feb 2009 11:20:59 GMT\n"
                                        "Content-Type: text/html; charset=utf-8\n"
                                        "Content-Length: 14\n"
                                        "Connection: close\n"
                                        "\n"
                                        "<h1>HELLO WORLD</h1>\n";

class ThreadPool
{
public:

    ThreadPool()
        : _threads_run(true)
        , _tasks_lock((pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER)
        , _completed_lock((pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER)
    {
        const long int  threads_count = sysconf(_SC_NPROCESSORS_ONLN);

        threadStart(threads_count);

        try {
            // TODO:move to upper level
        }
        catch (std::bad_alloc &e){
            logger::error("bad_alloc while trying to allocate thread array");
        }
    }

    ~ThreadPool()
    {
        _threads_run = false;

        std::for_each(_threads.begin(), _threads.end(), pthread_cancel);
        if (errno)
            logger::cerrno();

        pthread_mutex_destroy(&_tasks_lock);
        pthread_mutex_destroy(&_completed_lock);
    }

    void    addTaskToQueue(std::pair<int, HTTPMessage &> message_to_process);


private:
    ThreadPool(const ThreadPool &src);

    void    threadStart(long int threads_count)
    {
        _threads.reserve(threads_count);

        for (long int i = 0; i < threads_count; ++i)
        {
            pthread_t thread;

            if (pthread_create(&thread, NULL, threadLoop, this))
            {
                logger::cerrno(i);
                logger::error("Insufficient resources to create thread #", i);
                continue ;
            }
            if (pthread_detach(thread))
            {
                logger::cerrno(i);
                logger::error("Error while trying to detach thread #", i);
            }
            _threads.push_back(thread);
        }
        if (_threads.empty())
            throw std::bad_alloc();

        logger::info("Number of spawned threads:", _threads.size());
    }

    static void *threadLoop(void *thread_pool_void)
    {
        std::pair<int, HTTPMessage &> task;
        ThreadPool        *thread_pool     = reinterpret_cast<ThreadPool *>(thread_pool_void);
        pthread_mutex_t   *task_lock       = &thread_pool->_tasks_lock;
        pthread_cond_t    *task_event      = &thread_pool->_task_event;
        pthread_mutex_t   *completed_lock  = &thread_pool->_completed_lock;

        while (thread_pool->_threads_run)
        {
            pthread_mutex_lock(task_lock);
            {
                while (thread_pool->_tasks.empty())
                    pthread_cond_wait(task_event, task_lock);

                task = thread_pool->_tasks.front();
                thread_pool->_tasks.pop();
            }
            pthread_mutex_unlock(task_lock);

            task.second.raw_data = DefaultResponse; // TODO: process task here

            pthread_mutex_lock(completed_lock);
            {
                thread_pool->_completed.push(task);
            }
            pthread_mutex_unlock(completed_lock);
        }
        logger::debug("Worker stopped");
    }

    bool                                        _threads_run;
    std::vector<pthread_t>                      _threads;
    std::queue<std::pair<int, HTTPMessage &> >  _tasks;
    pthread_mutex_t                             _tasks_lock;
    pthread_cond_t                              _task_event;
    std::queue<std::pair<int, HTTPMessage &> >  _completed;
    pthread_mutex_t                             _completed_lock;
};

#endif
