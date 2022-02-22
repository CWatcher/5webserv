
#ifndef LOGGING_HPP
# define LOGGING_HPP

# include "utils.hpp"

# include <cstdio>

# define COLOR_WHITE	"\033[1;37m"
# define COLOR_YELLOW	"\033[0;33m"
# define COLOR_RED		"\033[0;31m"
# define COLOR_RESET	"\033[0m"

class Logging
{
public:
	enum LoggingLevel {
		Debug = 0,
		Info,
		Warning,
		Error
	};

	static void	debug(const std::string &str, ssize_t num = 0);
	static void	info(const std::string &str, ssize_t num = 0);
	static void	warning(const std::string &str, ssize_t num = 0);
	static void	error(const std::string &str, ssize_t num = 0);
	static void errno_error();

	static void setLevel(enum LoggingLevel level);
	static void setLevel(const std::string &level);

private:
	Logging();
	Logging(const Logging &src);
	Logging &operator=(const Logging &src);
	~Logging();

	static char *getTimestamp();
	static void	printLogLine(
		const std::string &level, const std::string &str, ssize_t num = 0
	);

	static enum LoggingLevel	logging_level;
};

typedef Logging log;

#endif
