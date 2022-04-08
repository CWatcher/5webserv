#pragma once

#include <pthread.h>

namespace ft
{

class mutex
{
public:
    typedef pthread_mutex_t native_handle_type;

    mutex()
    {
        pthread_mutexattr_init(&_mutexattr);
        pthread_mutex_init(&_mutex, &_mutexattr);
    }

    ~mutex()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_mutexattr_destroy(&_mutexattr);
    }

    void lock()
    {
        pthread_mutex_lock(&_mutex);
    }

    void unlock()
    {
        pthread_mutex_unlock(&_mutex);
    }

    bool try_lock()
    {
        return pthread_mutex_trylock(&_mutex);
    }

    native_handle_type native_handle() { return _mutex; }

private:
    pthread_mutex_t _mutex;
    pthread_mutexattr_t _mutexattr;
};

class spin_lock
{
public:
    typedef pthread_spinlock_t native_handle_type;

    spin_lock()
    {
        pthread_spin_init(&_spin_lock, 0);
    }

    ~spin_lock()
    {
        pthread_spin_destroy(&_spin_lock);
    }

    void lock()
    {
        pthread_spin_lock(&_spin_lock);
    }

    void unlock()
    {
        pthread_spin_unlock(&_spin_lock);
    }

    bool try_lock()
    {
        return pthread_spin_trylock(&_spin_lock);
    }

    native_handle_type native_handle() { return _spin_lock; }

private:
    pthread_spinlock_t _spin_lock;
};

//class recursive_mutex
//{
//private:
//    mutex _m;
//};

template<class Mutex>
class lock_guard
{
    lock_guard(lock_guard const& mutex);
    lock_guard& operator= (lock_guard const&);
public:
    typedef Mutex mutex_type;

    lock_guard(Mutex& mutex) : m(mutex) { m.lock(); }
    ~lock_guard() { m.unlock(); }
protected:
    mutex_type& m;
};

}
