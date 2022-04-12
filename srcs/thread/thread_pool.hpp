#pragma once

#include <queue>

#include "thread/thread.hpp"
#include "thread/semaphore.hpp"
#include "thread/mutex.hpp"
#include "thread/atomic.hpp"
#include "utils/array.hpp"

namespace ft
{

template<typename T>
class sem_queue
{
public:
    typedef  std::queue<T>                                      container_type;
    typedef  typename container_type::value_type                value_type;
    typedef  typename container_type::size_type                 size_type;
    typedef  typename container_type::reference                 reference;
    typedef  typename container_type::const_reference           const_reference;
public:
    void push(T const& task)
    {
        ft::lock_guard<ft::spin_lock> lock(_mutex);
        _queue.push(task);
        _sem.post();
    }

    T pull()
    {
        _sem.wait();
        ft::lock_guard<ft::spin_lock> lock(_mutex);
        T tmp = _queue.front();
        _queue.pop();
        return tmp;
        
    }

    bool empty() const
    {
        ft::lock_guard<ft::spin_lock> lock(_mutex);
        bool empty = _queue.empty();
        return empty;
    }

    size_type size() const
    {
        ft::lock_guard<ft::spin_lock> lock(_mutex);
        size_type size = _queue.size();
        return size;
    }
private:
    mutable ft::semaphore _sem;
    mutable ft::spin_lock _mutex;
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
    typedef ft::array<ft::thread, NThread>  thread_pool_type;
    typedef sem_queue<task>                 task_pool_type;

    struct thread_data
    {
        thread_data(task_pool_type& task_pool)
            : task_pool(task_pool) {}
        task_pool_type&                 task_pool;
    };

    static void* thread_loop(thread_data* d)
    {
        while (true)
        {
            task t = d->task_pool.pull();
            (*t.routine)(t.data);
        }
        return NULL;
    }

    static void* exit_task(void*)
    {
        ft::this_thread::exit();
        return NULL;
    }

    static void start_thread(ft::thread& runner, thread_data& thread_data)
    {
        runner = ft::thread(
                (ft::thread::routine_type)thread_loop,
                (ft::thread::data_type)&thread_data);
    }

public:
    thread_pool()
        : _thread_data(_task_pool)
    {
        for (typename thread_pool_type::iterator it = _thread_pool.begin();
                it != _thread_pool.end(); ++it)
            start_thread(*it, _thread_data);
    }

    ~thread_pool()
    {
        this->cancel();
    }

    template<typename Routine, typename Data>
    void push_task(Routine routine, Data data)
    {
        _task_pool.push(task(reinterpret_cast<ft::thread::routine_type>(routine),
                             reinterpret_cast<ft::thread::data_type>(data)));
    }

    void push_task(ft::thread::routine_type routine, ft::thread::data_type data)
    {
        _task_pool.push(task(routine, data));
    }

    void soft_stop()
    {
        for (size_t i = 0; i < NThread; i++)
            _task_pool.push(task(exit_task, NULL));
        for (typename thread_pool_type::iterator it = _thread_pool.begin();
                it != _thread_pool.end(); ++it)
        {
            if (it->joinable())
                it->join();
        }
    }

    void cancel()
    {
        for (typename thread_pool_type::iterator it = _thread_pool.begin();
                it != _thread_pool.end(); ++it)
        {
            if (it->joinable())
                it->cancel();
        }
    }

private:
    thread_pool_type        _thread_pool;
    task_pool_type          _task_pool;
    thread_data             _thread_data;
};

}
