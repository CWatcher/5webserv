
#include "Logger.hpp"

#include <cerrno>
#include <cstring>
#include <ctime>
#include <iostream>
#include <string>
#include <sys/time.h>

enum Logger::LoggingLevel	Logger::_logging_level = Logger::Debug;

void	Logger::setLevel(enum LoggingLevel level)
{
	_logging_level = level;
}

void	Logger::setLevel(const std::string &level)
{
	std::string level_lowercase = strLowerCaseCopy(level);

	if (level_lowercase == "debug")
		setLevel(Debug);
	else if (level_lowercase == "info")
		setLevel(Info);
	else if (level_lowercase == "warning")
		setLevel(Warning);
	else if (level_lowercase == "error")
		setLevel(Error);
	else
		std::cerr << "ERROR: Unknown Logging Level (Debug/Info/Warning/Error)" << std::endl;
}

void	Logger::debug(const std::string &message, ssize_t num)
{
	printLogLine(Debug, message, num);
}

void	Logger::info(const std::string &message, ssize_t num)
{
	printLogLine(Info, message, num);
}

void	Logger::warning(const std::string &message, ssize_t num)
{
	printLogLine(Warning, message, num);
}

void	Logger::error(const std::string &message, ssize_t num)
{
	printLogLine(Error, message, num);
}

void	Logger::cerrno(ssize_t num)
{
	printLogLine(Error, strerror(errno), num);
	errno = 0;
}

void	Logger::printLogLine(enum LoggingLevel msg_level, const std::string &message, ssize_t num)
{
	if (_logging_level <= msg_level)
	{
		std::cerr << timestamp() << " - " << _level_to_str[msg_level] << " - " << message;
		if (num)
			std::cerr << " " << num;
		std::cerr << std::endl;
	}
}

char	*Logger::timestamp()
{
	struct timeval	time_now;
	static char		time_str[16];
	char			msec[3];

	gettimeofday(&time_now, NULL);
	strftime(time_str, sizeof(time_str), "%H:%M:%S.", localtime(&time_now.tv_sec));

	msec[0] = static_cast<char>('0' + time_now.tv_usec / 100000 % 10);
	msec[1] = static_cast<char>('0' + time_now.tv_usec / 10000 % 10);
	msec[2] = static_cast<char>('0' + time_now.tv_usec / 1000 % 10);
	std::strncat(time_str, msec, 3);

	return time_str;
}

Logger::Logger() {}
Logger::Logger(const Logger &_) {(void)_;}
Logger &Logger::operator=(const Logger &_) {(void)_; return *this;}
Logger::~Logger() {}

# define COLOR_WHITE	"\033[1;37m"
# define COLOR_YELLOW	"\033[0;33m"
# define COLOR_RED		"\033[0;31m"
# define COLOR_RESET	"\033[0m"

const char*	Logger::_level_to_str[4] = {
				 "DEBUG",
	COLOR_WHITE  "INFO"    COLOR_RESET,
	COLOR_YELLOW "WARNING" COLOR_RESET,
	COLOR_RED    "ERROR!"  COLOR_RESET
};

# undef COLOR_WHITE
# undef COLOR_YELLOW
# undef COLOR_RED
# undef COLOR_RESET
