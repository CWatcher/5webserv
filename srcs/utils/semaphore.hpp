#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include <semaphore.h>
#include "utils/syntax.hpp"

namespace ft
{

class semaphore
{
public:
    semaphore(int value = 0) { EPROTECT_R(sem_init(&_semaphore, 0, value)); }
    ~semaphore() { sem_destroy(&_semaphore); }

    void wait() { EPROTECT_R(sem_wait(&_semaphore)); }
    bool trywait()
    {
        int status = sem_trywait(&_semaphore);
        if (status != EBUSY)
            EPROTECT_R(status);
        return status == 0;
    }
    void post() { EPROTECT_R(sem_post(&_semaphore)); }
    int get_value() { int value; EPROTECT(sem_getvalue(&_semaphore, &value)); return value; }

private:
    sem_t _semaphore;
};

}

#endif
