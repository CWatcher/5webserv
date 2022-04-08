#pragma once

#include <array>
#include <queue>

#include "thread/thread.hpp"
#include "thread/semaphore.hpp"
#include "thread/mutex.hpp"

namespace ft
{

template<typename T>
class sem_queue
{
public:
    void push(T const& task)
    {
        ft::lock_guard<ft::spin_lock>(_mutex);
        _queue.push(task);
        _sem.post();
    }

    T pull()
    {
        _sem.wait();
        ft::lock_guard<ft::spin_lock>(_mutex);
        T tmp = _queue.back();
        _queue.pop();
        return tmp;
        
    }
private:
    ft::semaphore _sem;
    ft::spin_lock _mutex;
    std::queue<T> _queue;
};

template<size_t NThread = 8>
class thread_pool
{

    struct task
    {
        ft::thread::routine_type routine;
        ft::thread::data_type    data;
    };

    struct thread
    {
        static void run(sem_queue* task_pool)
        {
            while (true)
            {
                task t  = task_pool->pull();
                (*t.routine)(t.data);
            }
        }

        void run(sem_queue<task> task_pool)
        {
            _thread = ft::thread(run, &task_pool);
        }

    private:
        ft::thread _thread;
    };
public:

private:
    std::array<ft::thread_pool::thread, NThread>    thread_pool;
    sem_queue<task>                                 task_pool;
};

}
