
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
Level   getLevel();

template<logger::Level::_ Lvl>
class LogStream : public std::ostream
{
public:
    bool isLogging() const;

    std::ostream& base() const;
    std::ostream& devnull() const;

    template<typename Any>
    basic_ostream&    operator<<(Any);
};

class DevnullStream : public std::ostream { };

std::ostream&   baseStream();
std::ostream&   devnullStream();

extern LogStream<logger::Level::kInfo>     info;
extern LogStream<logger::Level::kDebug>    debug;
extern LogStream<logger::Level::kWarning>  warning;
extern LogStream<logger::Level::kError>    error;

template<logger::Level::_ Lvl>
bool    LogStream<Lvl>::isLogging() const
{
    return logger::getLevel() <= Lvl;
}

template<logger::Level::_ Lvl>
std::ostream& LogStream<Lvl>::base() const
{
    return logger::baseStream();
}

template<logger::Level::_ Lvl>
std::ostream& LogStream<Lvl>::devnull() const
{
    return logger::devnullStream();
}

void printLoggerInfo(logger::Level lvl);

template<logger::Level::_ Lvl>
template<typename Any>
typename LogStream<Lvl>::basic_ostream&    LogStream<Lvl>::operator<<(Any data)
{
    if (isLogging() == false)
        return devnull();
    printLoggerInfo(Lvl);
    base() << data;
    return base();
}

}

#endif
