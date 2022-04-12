
#include "ThreadPool.hpp"
#include "Logger.hpp"

#include <pthread.h>
#include <unistd.h>
#include <algorithm>
#include <cerrno>
#include <utility>

ThreadPool::ThreadPool()
    : _is_running(true)
    , _tasks_lock((pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER)
    , _tasks_event((pthread_cond_t)PTHREAD_COND_INITIALIZER)
{
    long int  threads_count = sysconf(_SC_NPROCESSORS_ONLN);

    if (threads_count == 0)
    {
        logger::warning("ThreadPool: tried to spawn 0 threads");
        threads_count = 1;
    }

    threadsStart(threads_count);
}

ThreadPool::~ThreadPool()
{
    void *null;

    _is_running = false;

    logger::info("ThreadPool: shutting down...");
    std::for_each(_threads.begin(), _threads.end(), pthread_cancel);
    if (errno)
        logger::cerrno();

    logger::debug("ThreadPool: waiting for threads...");
    for (std::vector<pthread_t>::const_iterator it = _threads.begin(); it != _threads.end(); ++it)
    {
        logger::debug("Joining thread...");
        pthread_join(*it, &null);
    }
    pthread_cond_destroy(&_tasks_event);
    pthread_mutex_destroy(&_tasks_lock);
    logger::info("ThreadPool: shutdown completed");
}

void    ThreadPool::pushTaskToQueue(SocketSession *task)
{
    pthread_mutex_lock(&_tasks_lock);
    {
        _tasks.push(task);
    }
    pthread_mutex_unlock(&_tasks_lock);
    pthread_cond_signal(&_tasks_event);
};

void    ThreadPool::threadsStart(long int threads_count)
{
    for (long int i = 0; i < threads_count; ++i)
    {
        pthread_t thread;

        if (pthread_create(&thread, NULL, threadLoop, this))
        {
            logger::cerrno(i);
            logger::error("ThreadPool: insufficient resources to create thread #", i);
            continue ;
        }
        try {
            _threads.push_back(thread);
        }
        catch (std::bad_alloc &e){
            logger::error("bad_alloc while trying to push_back to thread array");
        }
    }
    if (_threads.empty())
        throw std::bad_alloc();

    logger::info("ThreadPool: number of spawned threads:", _threads.size());
}

void    *ThreadPool::threadLoop(void *thread_pool_void)
{
    SocketSession   *task;
    ThreadPool      *thread_pool = reinterpret_cast<ThreadPool *>(thread_pool_void);
    pthread_mutex_t *task_lock   = &thread_pool->_tasks_lock;
    pthread_cond_t  *task_event  = &thread_pool->_tasks_event;

    while (thread_pool->_is_running)
    {
        pthread_mutex_lock(task_lock);
        {
            pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock, task_lock);

            logger::debug("Thread: task wait...");
            while (thread_pool->_tasks.empty())
                pthread_cond_wait(task_event, task_lock);

            task = thread_pool->_tasks.front();
            thread_pool->_tasks.pop();
            logger::debug("Thread: task got");

            pthread_cleanup_pop(1);
        }
        pthread_mutex_unlock(task_lock);

        // TODO: process task input here
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
        task->output.raw_data = DefaultResponse;

        task->prepareForWrite();
        logger::debug("Thread: task completed");
    }
    logger::debug("Thread: stopped");
    return NULL;
}


//#include <iostream>
//
//int main()
//{
//    SocketSession socket(0);
//    ThreadPool pool;
//
//    socket.input.raw_data = "OK";
//    pool.pushTaskToQueue(&socket);
//    sleep(1);
//    std::cout << socket.input.raw_data << std::endl << socket.output.raw_data << std::endl;
//
//    sleep(1);
//
//    socket.input.raw_data = "OKKOKOKO";
//    pool.pushTaskToQueue(&socket);
//    sleep(1);
//    std::cout << socket.input.raw_data << std::endl << socket.output.raw_data << std::endl;
//}
