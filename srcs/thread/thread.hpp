#pragma once

#include <pthread.h>

namespace ft
{

class thread
{
public:
    struct id
    {
    private:
        pthread_t _id;
        friend class thread;
    };

    thread() { }

    typedef void*(*routine_type)(void*);
    typedef void* data_type;

    thread(routine_type routine, data_type data = NULL)
    {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&_id._id, &attr, routine, data);
        pthread_attr_destroy(&attr);
    }

    ~thread()
    {
    }

    bool joinable() const
    {
        
    }

    id get_id() const
    {
        return _id;
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

}
