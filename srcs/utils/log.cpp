#include <fstream>
#include <ctime>
#include <sys/time.h>
#include <cerrno>
#include <cstring>
#include <pthread.h>

#include "log.hpp"

namespace logger
{

LogStream<logger::Level::kInfo>     info;
LogStream<logger::Level::kDebug>    debug;
LogStream<logger::Level::kWarning>  warning;
LogStream<logger::Level::kError>    error;

DevnullStream                       dev_null;
std::ostream*                       ostream = &std::clog;
logger::Level                       level = logger::Level::kDebug;

Errno  cerror;
EndLog end;

namespace {
pthread_mutex_t logger_lock = PTHREAD_MUTEX_INITIALIZER;
bool            is_locked;
}

Level   getLevel()
{
    return level;
}

std::ostream&   baseStream()
{
    return *ostream;
}

std::ostream&   devnullStream()
{
    return dev_null;
}

void    setLevel(logger::Level level)
{
    logger::level = level;
}

void    setLevel(const std::string &level)
{
    if      (level == "debug")
        setLevel(logger::Level::kDebug);
    else if (level == "info")
        setLevel(logger::Level::kInfo);
    else if (level == "warning")
        setLevel(logger::Level::kWarning);
    else if (level == "error")
        setLevel(logger::Level::kError);
    else
        throw std::logic_error("invalid debug level name");
}

void    setOut(std::ostream& ostream)
{
    logger::ostream = &ostream;
}

void    setOut(std::string const& ostream_name)
{
    if      (ostream_name == "cout")
        logger::setOut(std::cout);
    else if (ostream_name == "cerr")
        logger::setOut(std::cerr);
    else if (ostream_name == "clog")
        logger::setOut(std::clog);
    else
    {
        static std::ofstream fout;
        if (fout.is_open())
            fout.close();
        fout.open(ostream_name.c_str());
        logger::setOut(fout);
    }
}

namespace {

std::string   timestamp()
{
    timeval         time_now;
    char            time_str[16];
    char            msec[3];

    gettimeofday(&time_now, NULL);
    strftime(time_str, sizeof(time_str), "%H:%M:%S.", localtime(&time_now.tv_sec));

    msec[0] = static_cast<char>('0' + time_now.tv_usec / 100000 % 10);
    msec[1] = static_cast<char>('0' + time_now.tv_usec / 10000 % 10);
    msec[2] = static_cast<char>('0' + time_now.tv_usec / 1000 % 10);
    std::strncat(time_str, msec, 3);

    return time_str;
}


# define COLOR_WHITE    "\033[1;37m"
# define COLOR_YELLOW   "\033[0;33m"
# define COLOR_RED      "\033[0;31m"
# define COLOR_RESET    "\033[0m"

const char* lvlToStr[4] = {
                 "DEBUG",
    COLOR_WHITE  "INFO"    COLOR_RESET,
    COLOR_YELLOW "WARNING" COLOR_RESET,
    COLOR_RED    "ERROR!"  COLOR_RESET
};

}

void printLoggerInfo(logger::Level lvl)
{
    baseStream() << '[' << logger::timestamp() << ']'
        << " - " << lvlToStr[lvl] << " - ";
}

void lockOut()   { pthread_mutex_lock(&logger_lock); is_locked = true; }
void unlockOut() { is_locked = false;  pthread_mutex_unlock(&logger_lock); }

std::ostream& operator<<(std::ostream& o, EndLog& endlog)
{
    (void)endlog;
    o << '\n';
    if (logger::is_locked == true)
        unlockOut();
    return o;
}

std::ostream& operator<<(std::ostream& o, Errno&) { return o << strerror(errno); }

}
