
#include "Logging.hpp"

#include <cerrno>
#include <cstring>
#include <ctime>
#include <sys/time.h>
#include <iostream>
#include <string>

enum Logging::LoggingLevel	Logging::logging_level = Logging::Debug;

void	Logging::debug(const std::string &str, ssize_t num)
{
	if (logging_level <= Debug)
		printLogLine("DEBUG", str, num);
}

void	Logging::info(const std::string &str, ssize_t num)
{
	if (logging_level <= Info)
		printLogLine(COLOR_WHITE "INFO" COLOR_RESET, str, num);
}

void	Logging::warning(const std::string &str, ssize_t num)
{
	if (logging_level <= Warning)
		printLogLine(COLOR_YELLOW "WARNING" COLOR_RESET, str, num);
}

void	Logging::error(const std::string &str, ssize_t num)
{
	if (logging_level <= Error)
		printLogLine(COLOR_RED "ERROR!" COLOR_RESET, str, num);
}

void	Logging::errno_error()
{
	if (errno && logging_level <= Error)
		printLogLine(COLOR_RED "ERRNO!" COLOR_RESET, strerror(errno), errno);
	errno = 0;
}

void	Logging::setLevel(enum LoggingLevel level)
{
	logging_level = level;
}

void	Logging::setLevel(const std::string &level)
{
	std::string level_lowercase = strLowerCaseCopy(level);

	if (level_lowercase == "debug")
		logging_level = Debug;
	else if (level_lowercase == "info")
		logging_level = Info;
	else if (level_lowercase == "warning")
		logging_level = Warning;
	else if (level_lowercase == "error")
		logging_level = Error;
	else
		std::cerr << "ERROR: Unknown Logging Level (Debug/Info/Warning/Error)"
					<< std::endl;
}

char	*Logging::getTimestamp()
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

void	Logging::printLogLine(const std::string &level, const std::string &str, ssize_t num)
{
	std::cerr << getTimestamp() << " - " << level << " - " << str;
	if (num)
		std::cerr << " " << num;
	std::cerr << std::endl;
}

Logging::Logging() {}
Logging::Logging(const Logging &_) {(void)_;}
Logging &Logging::operator=(const Logging &_) {(void)_; return *this;}
Logging::~Logging() {}
