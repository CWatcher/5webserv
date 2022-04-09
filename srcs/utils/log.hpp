
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

std::ostream& put(Level level, std::string const& msg);
std::ostream& puterrno(Level level = Level::kError);

inline std::ostream& debug(std::string const& msg) { return put(Level::kDebug, msg); }
inline std::ostream& info(std::string const& msg) { return put(Level::kInfo, msg); }
inline std::ostream& warning(std::string const& msg) { return put(Level::kWarning, msg); }
inline std::ostream& error(std::string const& msg) { return put(Level::kError, msg); }

}

#endif
