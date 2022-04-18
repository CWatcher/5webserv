#pragma once

#include <queue>
#include <vector>
#include <unistd.h>

#include "utils/thread.hpp"
#include "utils/semaphore.hpp"
#include "utils/mutex.hpp"
#include "utils/atomic.hpp"
#include "utils/array.hpp"

namespace ft
{

template<typename T>
class wait_queue
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
        {
            ft::lock_guard<mutex> lock(_mutex);
            _queue.push(task);
        }
        _cond.signal();
    }

    T pull()
    {
        ft::lock_guard<mutex> lock(_mutex);
        while (empty() == true)
            _cond.wait(_mutex);
        T tmp = _queue.front();
        _queue.pop();
        return tmp;
    }

    bool empty() const
    {
        bool empty = _queue.empty();
        return empty;
    }

    size_type size() const
    {
        size_type size = _queue.size();
        return size;
    }
private:
    mutable ft::cond_var    _cond;
    mutable ft::mutex       _mutex;
    std::queue<T>           _queue;
};


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

    typedef std::vector<ft::thread>         thread_pool_type;
    typedef wait_queue<task>                task_queue_type;

    static void* thread_loop(task_queue_type* task_pool)
    {
        while (true)
        {
            task t = task_pool->pull();
            (*t.routine)(t.data);
        }
        return NULL;
    }

    static void* exit_task(void*)
    {
        ft::this_thread::exit();
        return NULL;
    }

    static void start_thread(ft::thread& runner, task_queue_type& task_pool)
    {
        runner = ft::thread(
                (ft::thread::routine_type)thread_loop,
                (ft::thread::data_type)&task_pool);
    }

public:
    thread_pool(size_t n = sysconf(_SC_NPROCESSORS_ONLN))
    {
        _thread_pool.resize(n);
        for (typename thread_pool_type::iterator it = _thread_pool.begin();
                it != _thread_pool.end(); ++it)
        {
            start_thread(*it, _task_queue);
        }
    }

    void resize(size_t n)
    {
        for (size_t i = n; i < _thread_pool.size(); ++i)
        {
            _thread_pool.push_back(ft::thread());
            start_thread(_thread_pool.back(), _task_queue);
        }
        for (size_t i = n; i > _thread_pool.size(); --i)
        {
            if (_thread_pool[i].joinable())
                _thread_pool[i].cancel();
        }
    }

    ~thread_pool()
    {
        this->cancel();
    }

    template<typename Routine, typename Data>
    void push_task(Routine routine, Data data)
    {
        _task_queue.push(task(reinterpret_cast<ft::thread::routine_type>(routine),
                             reinterpret_cast<ft::thread::data_type>(data)));
    }

    void push_task(ft::thread::routine_type routine, ft::thread::data_type data)
    {
        _task_queue.push(task(routine, data));
    }

    void soft_stop()
    {
        for (size_t i = 0; i < _thread_pool.size(); i++)
            _task_queue.push(task(exit_task, NULL));

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
    task_queue_type          _task_queue;
};

}
