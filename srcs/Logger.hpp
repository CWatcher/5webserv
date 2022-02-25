
#ifndef LOGGER_HPP
# define LOGGER_HPP

# include "utils.hpp"

# include <cstdio>

class Logger
{
public:
	enum LoggingLevel {
		Debug = 0,
		Info,
		Warning,
		Error
	};

	static void	debug(const std::string &message, ssize_t num = 0);
	static void	info(const std::string &message, ssize_t num = 0);
	static void	warning(const std::string &message, ssize_t num = 0);
	static void	error(const std::string &message, ssize_t num = 0);
	static void cerrno(ssize_t num = 0);

	static void setLevel(enum LoggingLevel level);
	static void setLevel(const std::string &level);

private:
	Logger();
	Logger(const Logger &_);
	Logger &operator=(const Logger &_);
	~Logger();

	static char	*timestamp();
	static void	printLogLine(
		enum LoggingLevel msg_level, const std::string &message, ssize_t num = 0
	);

	static enum LoggingLevel	_logging_level;
	static const char*			_level_to_str[4];
};

typedef Logger	logger;

#endif
