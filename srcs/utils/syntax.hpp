#ifndef SYNTAX_HPP
#define SYNTAX_HPP

#include <cstring>
#include <errno.h>

#define EPROTECT_R(op) { int __s = op; if (__s != 0) throw std::runtime_error(strerror(__s)); } while (0)
#define EPROTECT(op) { int __s = op; if (__s != 0) throw std::runtime_error(strerror(errno)); } while (0)

#endif
