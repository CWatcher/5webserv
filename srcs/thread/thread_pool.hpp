#pragma once

#include <array>
#include <queue>

#include "thread/thread.hpp"
#include "thread/semaphore.hpp"
#include "thread/mutex.hpp"
#include "thread/atomic.hpp"

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

    bool empty() const
    {
        ft::lock_guard<ft::spin_lock>(_mutex);
        bool empty = _queue.empty()
        return empty;
    }

    std::queue<T>::size_type size() const
    {
        ft::lock_guard<ft::spin_lock>(_mutex);
        std::queue<T>::size_type size = _queue.size();
        return size;
    }
private:
    ft::semaphore _sem;
    ft::spin_lock _mutex;
    std::queue<T> _queue;
};


template<size_t NThread = 8>
class thread_pool
{
private:
    struct task
    {
        task(ft::thread::routine_type routine, ft::thread::data_type data)
            : routine(routine), data(data) {}
        ft::thread::routine_type routine;
        ft::thread::data_type    data;
    };


private:
    typedef std::array<ft::thread, NThread> thread_pool_type;
    typedef sem_queue<task>                 task_pool_type;

    struct thread_data
    {
        thread_data(task_pool_type& task_pool, ft::fake_atomic<bool> const& wait_exit)
            : task_pool(task_pool), wait_exit(wait_exit) {}
        task_pool_type&                 task_pool;
        ft::fake_atomic<bool> const&    wait_exit;
    };

    static void thread_loop(thread_data* d)
    {
        while (d->wait_exit == false || d->task_pool.empty() == true)
        {
            task t = task_pool->pull();
            (*t.routine)(t.data);
        }
    }

    static void start_thread(ft::thread& runner, thread_data& thread_data)
    {
        runner = ft::thread(thread_loop, &thread_data);
    }

public:
    thread_pool()
        : _wait_exit(false), _thread_data(_task_pool, _wait_exit)
    {
        for (thread_pool_type::iterator it = _thread_pool.begin();
                it != _thread_pool.end(); ++it)
            start_thread(*it, _thread_data);
    }

    ~thread_pool()
    {
        this->cancel();
    }

    void push_task(ft::thread::routine_type routine, ft::thread::data_type data)
    {
        _task_pool.push(task(routine, data));
    }

    void soft_stop()
    {
        _wait_exit.set(true);
        for (thread_pool_type::iterator it = _thread_pool.begin();
                it != _thread_pool.end(); ++it)
            it->join();
    }

    void cancel()
    {
        for (thread_pool_type::iterator it = _thread_pool.begin();
                it != _thread_pool.end(); ++it)
            it->cancel();
    }

private:
    thread_pool_type        _thread_pool;
    task_pool_type          _task_pool;
    ft::fake_atomic<bool>   _wait_exit;
    thread_data             _thread_data;
};

}
