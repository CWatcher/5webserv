
#ifndef LOG_HPP
# define LOG_HPP

# include <cstdio>
# include <iostream>

# include "utils/syntax.hpp"

namespace logger
{

enum_class(Level)
    kDebug = 0,
    kInfo,
    kWarning,
    kError
enum_class_end

void    setLevel(Level level);
void    setLevel(std::string const& level);
void    setOut(std::ostream& ostream);
void    setOut(std::string const& ostream_name);

std::ostream& put(Level level);
std::ostream& puterrno(Level level = Level::kError);

inline std::ostream& debug() { return put(Level::kDebug); }
inline std::ostream& info() { return put(Level::kInfo); }
inline std::ostream& warning() { return put(Level::kWarning); }
inline std::ostream& error() { return put(Level::kError); }

}

#endif
