
#ifndef LOG_HPP
# define LOG_HPP

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

std::ostream&   baseStream();
std::ostream&   devnullStream();
void lockOut();
void unlockOut();

template<logger::Level::_ Lvl>
class LogStream : public std::ostream
{
public:
    template<typename Any>
    basic_ostream&    operator<<(Any);

    bool            isLogging() const { return logger::getLevel() <= Lvl; }
    std::ostream&   base() const    { return baseStream(); }
    std::ostream&   devnull() const { return devnullStream(); }
};

class DevnullStream : public std::ostream { };

extern LogStream<logger::Level::kInfo>     info;
extern LogStream<logger::Level::kDebug>    debug;
extern LogStream<logger::Level::kWarning>  warning;
extern LogStream<logger::Level::kError>    error;

void printLoggerInfo(logger::Level lvl);

template<logger::Level::_ Lvl>
template<typename Any>
typename LogStream<Lvl>::basic_ostream&    LogStream<Lvl>::operator<<(Any data)
{
    if (isLogging() == false)
        return devnull();
    lockOut();
    printLoggerInfo(Lvl);
    base() << data;
    return base();
}

class EndLog
{
    friend std::ostream& operator<<(std::ostream& o, EndLog& endlog);
};

extern EndLog   end;

class Errno { };
std::ostream& operator<<(std::ostream& o, Errno&);
extern Errno    cerror;

}

#endif
