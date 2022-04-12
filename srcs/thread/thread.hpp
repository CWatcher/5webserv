#pragma once

#include <pthread.h>

namespace ft
{

class thread
{
public:
    struct id
    {
        id(pthread_t id = 0) : _id(id) {}
        operator pthread_t () { return _id; }
    private:
        pthread_t _id;
        friend class thread;
    };

    typedef void*(*routine_type)(void*);
    typedef void* data_type;

    thread(routine_type routine, data_type data = NULL)
        : _joinable(true)
    {
        pthread_create(&_id._id, NULL, routine, data);
    }

    thread()  : _joinable(false), _id(0) { }
    ~thread() { }

    bool joinable() const { return _joinable; }

    id get_id() const { return _id; }

    void cancel()
    {
        pthread_cancel(_id);
        _joinable = false;
    }

    void join()
    {
        pthread_join(_id, NULL);
        _joinable = false;
    }

    void detach()
    {
        pthread_detach(_id);
        _joinable = false;
    }

public:
    typedef pthread_t native_handle_type;
private:
    bool _joinable;
    id   _id;
};

namespace this_thread
{

inline ft::thread::id get_id() { return pthread_self(); }
inline void exit(void *data = NULL) { pthread_exit(data); }

}

}
