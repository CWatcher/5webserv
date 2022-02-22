#include <algorithm>
#include <string>

std::string	&strTrim(std::string &str)
{
	const size_t	startpos = str.find_first_not_of(" \t");
	const size_t	endpos = str.find_last_not_of(" \t");

	if (startpos == std::string::npos)
		str.erase();
	else
		str = str.substr(startpos, endpos + 1);
	return str;
}

std::string	&strLowerCase(std::string &str)
{
	std::for_each(str.begin(), str.end(), tolower);
	return str;
}

std::string	strLowerCaseCopy(const std::string &str)
{
	std::string str_copy = str;

	std::for_each(str_copy.begin(), str_copy.end(), tolower);
	return str_copy;
}
