#pragma once

#include <pthread.h>

namespace ft
{

class thread
{
public:
    struct id
    {
        id(pthread_t id = 0) : _id(_id) {}
        operator pthread_t () { return _id; }
    private:
        pthread_t _id;
        friend class thread;
    };

    typedef void*(*routine_type)(void*);
    typedef void* data_type;

    thread(routine_type routine, data_type data = NULL)
    {
        pthread_create(&_id._id, NULL, routine, data);
    }

    thread()  { }
    ~thread() { }

    bool joinable() const
    {
        
    }

    id get_id() const
    {
        return _id;
    }

    void cancel()
    {
        pthread_cancel(_id._id);
    }

    void join()
    {
        pthread_join(_id._id, NULL);
    }

    void detach()
    {
        pthread_detach(_id._id);
    }

public:
    typedef pthread_t native_handle_type;
private:
    id _id;
};

namespace this_thread
{

inline ft::thread::id get_id()
{
    return pthread_self();
}

inline void exit(void *data = NULL)
{
    pthread_exit(data);
}

}

}
