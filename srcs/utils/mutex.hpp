#ifndef MUTEX_HPP
#define MUTEX_HPP
 
#include <pthread.h>

namespace ft
{

class mutex
{
public:
    typedef pthread_mutex_t native_handle_type;

    mutex() { pthread_mutex_init(&_mutex, NULL); }
    ~mutex() { pthread_mutex_destroy(&_mutex); }

    void lock() { pthread_mutex_lock(&_mutex); }
    void unlock() { pthread_mutex_unlock(&_mutex); }
    bool try_lock() { return pthread_mutex_trylock(&_mutex); }

    native_handle_type& native_handle() { return _mutex; }

private:
    pthread_mutex_t _mutex;
};

class spin_lock
{
public:
    typedef pthread_spinlock_t native_handle_type;

    spin_lock() { pthread_spin_init(&_spin_lock, 0); }
    ~spin_lock() { pthread_spin_destroy(&_spin_lock); }
    void lock() { pthread_spin_lock(&_spin_lock); }
    void unlock() { pthread_spin_unlock(&_spin_lock); }
    bool try_lock() { return pthread_spin_trylock(&_spin_lock); }

    native_handle_type& native_handle() { return _spin_lock; }

private:
    pthread_spinlock_t _spin_lock;
};

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

class cond_var
{
public:
    cond_var() { pthread_cond_init(&_cond, NULL); }
    ~cond_var() { pthread_cond_destroy(&_cond); }

    void broadcast() { pthread_cond_broadcast(&_cond); }
    void wait(ft::mutex& mut) { pthread_cond_wait(&_cond, &mut.native_handle()); }
    void signal() { pthread_cond_signal(&_cond); }

private:
    pthread_cond_t  _cond;
};

}

#endif
