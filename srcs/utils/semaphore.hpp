#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include <semaphore.h>

namespace ft
{

class semaphore
{
public:
    semaphore(int value = 0) { sem_init(&_semaphore, 0, value); }
    ~semaphore() { sem_destroy(&_semaphore); }

    void wait() { sem_wait(&_semaphore); }
    bool trywait() { return sem_trywait(&_semaphore); }
    void post() { sem_post(&_semaphore); }
    int get_value() { int value; sem_getvalue(&_semaphore, &value); return value; }

private:
    sem_t _semaphore;
};

}

#endif
