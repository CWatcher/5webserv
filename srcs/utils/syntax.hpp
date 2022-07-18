#ifndef SYNTAX_HPP
# define SYNTAX_HPP

# include <cstring>
# include <errno.h>

class __enum_class
{
public:
    __enum_class() {}
    __enum_class(int v) : _value(v) {}
    operator int() const { return _value; }

private:
    int _value;
};

# define enum_class(name)          \
class name : public __enum_class    \
{                                   \
public:                             \
    name() {}                       \
    name(int v)                     \
        : __enum_class(v) {}        \
    enum _                          \
    {

# define enum_class_end \
    };                   \
};


# define foreach(type, container, it_name) \
    for (type::iterator it_name = container.begin(); \
        it_name != container.end(); \
        it_name++)

# define cforeach(type, container, it_name) \
    for (type::const_iterator it_name = const_cast<type const&>(container).begin(); \
        it_name != const_cast<type const&>(container).end(); \
        it_name++)

# define rforeach(type, container, it_name) \
    for (type::reverse_iterator it_name = container.rbegin(); \
        it_name != container.rend(); \
        it_name++)

# define crforeach(type, container, it_name) \
    for (type::const_reverse_iterator it_name = const_cast<type const&>(container).rbegin(); \
        it_name != const_cast<type const&>(container).rend(); \
        it_name++)

# define EPROTECT_R(op) { int __s = op; if (__s != 0) throw std::runtime_error(strerror(__s)); } while (0)
# define EPROTECT(op) { int __s = op; if (__s != 0) throw std::runtime_error(strerror(errno)); } while (0)

#endif
