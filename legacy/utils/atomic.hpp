#ifndef ATOMIC_HPP
# define ATOMIC_HPP

#include "utils/mutex.hpp"

namespace ft
{

template<typename T>
class fake_atomic
{
public:
    typedef T value_type;

    fake_atomic(value_type data = T())
        : _data(data) {}

    value_type get() const
    {
        ft::lock_guard<ft::spin_lock> lock(_mutex);
        return _data;
    }

    void set(value_type const& v)
    {
        ft::lock_guard<ft::spin_lock> lock(_mutex);
        _data = v;
    }

    operator T() const { return get(); }

private:
    mutable ft::spin_lock   _mutex;
    value_type      _data;
};

}

#endif
