
#include "ThreadPool.hpp"
#include "handlers/starter.hpp"
//#include "Logger.hpp"

#include <pthread.h>
#include <unistd.h>
#include <algorithm>
#include <cerrno>

ThreadPool::ThreadPool()
    : _is_running(true)
    , _tasks_lock((pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER)
    , _tasks_event((pthread_cond_t)PTHREAD_COND_INITIALIZER)
{
    long int  threads_count = sysconf(_SC_NPROCESSORS_ONLN);

    if (threads_count == 0)
    {
//        logger::warning("ThreadPool: tried to spawn 0 threads");
        threads_count = 1;
    }

    threadsStart(threads_count);
}

ThreadPool::~ThreadPool()
{
    void *null;

    _is_running = false;

//    logger::info("ThreadPool: shutting down...");
    std::for_each(_threads.begin(), _threads.end(), pthread_cancel);
//    if (errno)
//        logger::cerrno();

//    logger::debug("ThreadPool: waiting for threads...");
    for (std::vector<pthread_t>::const_iterator it = _threads.begin(); it != _threads.end(); ++it)
    {
//        logger::debug("Joining thread...");
        pthread_join(*it, &null);
    }
    pthread_cond_destroy(&_tasks_event);
    pthread_mutex_destroy(&_tasks_lock);
//    logger::info("ThreadPool: shutdown completed");
}

void    ThreadPool::pushTaskToQueue(SocketSession *task)
{
    pthread_mutex_lock(&_tasks_lock);
    {
        _tasks.push(task);
    }
    pthread_mutex_unlock(&_tasks_lock);
    pthread_cond_signal(&_tasks_event);
}

void unlock(void *lock) {
    pthread_mutex_unlock(reinterpret_cast<pthread_mutex_t *>(lock));
}

SocketSession   *ThreadPool::popTaskFromQueue()
{
    SocketSession   *task;

    pthread_mutex_lock(&_tasks_lock);
    {
        pthread_cleanup_push(unlock, &_tasks_lock);

//            logger::debug("Thread: task wait...");
            while (_tasks.empty())
                pthread_cond_wait(&_tasks_event, &_tasks_lock);

            task = _tasks.front();
            _tasks.pop();
//            logger::debug("Thread: task got");

        pthread_cleanup_pop(1);
    }
    pthread_mutex_unlock(&_tasks_lock);

    return task;
}

void    ThreadPool::threadsStart(long int threads_count)
{
    for (long int i = 0; i < threads_count; ++i)
    {
        pthread_t thread;

        if (pthread_create(&thread, NULL, threadLoop, this))
        {
//            logger::cerrno(i);
//            logger::error("ThreadPool: insufficient resources to create thread #", i);
            continue ;
        }
        try {
            _threads.push_back(thread);
        }
        catch (std::bad_alloc &e){
//            logger::error("bad_alloc while trying to push_back to thread array");
        }
    }
    if (_threads.empty())
        throw std::bad_alloc();

//    logger::info("ThreadPool: number of spawned threads:", _threads.size());
}

void    *ThreadPool::threadLoop(void *thread_pool_void)
{
    SocketSession   *task;
    ThreadPool      *thread_pool = reinterpret_cast<ThreadPool *>(thread_pool_void);

    while (thread_pool->_is_running)
    {
        task = thread_pool->popTaskFromQueue();

        handlers::starter->handle(task->input, task->output);

        task->prepareForWrite();

//        logger::debug("Thread: task completed");
    }
//    logger::debug("Thread: stopped");
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
