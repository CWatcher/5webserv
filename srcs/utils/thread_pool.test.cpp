#include <iostream>
#include <sstream>

#include "utils/thread_pool.hpp"

size_t fact(size_t ii)
{
    if (ii <= 2)
        return ii;
    return fact(ii - 1) + fact(ii - 2);
}

static ft::spin_lock output;

size_t n;

void *work(void *data)
{
    size_t i = fact((size_t)data);
    std::stringstream s;
    s << n++ << ' ' << (size_t)data << ' ' << i << '\n';
    {
        ft::lock_guard<ft::spin_lock> lock(output);
        std::cout << s.str();
    }
    return NULL;
}

int main()
{
    ft::thread_pool<8> pool;
    for (size_t i = 0; i < 2; i++)
        pool.push_task(work, (void*)40);
    pool.cancel();
}