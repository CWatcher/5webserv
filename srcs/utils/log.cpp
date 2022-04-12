#include <fstream>
#include <ctime>
#include <sys/time.h>
#include <cerrno>
#include <cstring>

#include "log.hpp"

namespace logger
{

class DevNull : public std::ostream
{
    template<typename T>
    basic_ostream& operator<<(T value) { (void)value; return *this; }
};

static logger::Level  level   = logger::Level::kDebug;
static std::ostream*  ostream = &std::cerr;
static DevNull        dev_null;

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
    else
    {
        static std::ofstream fout;
        if (fout.is_open())
            fout.close();
        fout.open(ostream_name.c_str());
        logger::setOut(fout);
    }
}

char*   _time()
{
    struct timeval  time_now;
    static char     time_str[16];
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

std::ostream&    put(logger::Level level)
{
    if (logger::level > level)
        return dev_null;
    
    (*logger::ostream) << '\n' << logger::_time()
        << " - " << logger::lvlToStr[level] << " - ";
    return *logger::ostream;
}

std::ostream& puterrno(Level level)
{
    return put(level) << strerror(errno);
}

}
