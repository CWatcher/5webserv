#include "string.hpp"

#include <algorithm>
#include <sstream>

std::string	&strTrim(std::string &str, const char *s)
{
	const size_t	startpos = str.find_first_not_of(s);
	const size_t	endpos = str.find_last_not_of(s);

	if (startpos == std::string::npos)
		str.erase();
	else
		str = str.substr(startpos, endpos - startpos + 1);
	return str;
}

std::string	&strLowerCase(std::string &str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}

std::string	strLowerCaseCopy(const std::string &str)
{
	std::string str_copy = str;

	std::transform(str_copy.begin(), str_copy.end(), str_copy.begin(), ::tolower);
	return str_copy;
}

std::string	&strRemoveDoubled(std::string &str, char c)
{
	const char	twins[] = {c, c, '\0'};
	size_t		doubled = str.find(twins);

	while (doubled != std::string::npos)
	{
		str.erase(doubled, 1);
		doubled = str.find(twins);
	}
	return str;
}

std::string	&strCompleteWith(std::string &str, char c)
{
	if (*str.rbegin() != c)
		str.push_back(c);
	return str;
}

bool	strEndsWith(const std::string &str, const std::string &suffix)
{
	if (str.length() < suffix.length())
		return false;

	std::string::const_reverse_iterator	it2 = str.rbegin();
	for (std::string::const_reverse_iterator it1 = suffix.rbegin(); it1 != suffix.rend(); ++it1, ++it2)
		if (*it1 != *it2)
			return false;
	return true;
}

std::vector<std::string>	strSplit(const std::string &str, char delim)
{
	std::stringstream			ss(str);
	std::string					word;
	std::vector<std::string>	splitted;

	while (std::getline(ss, word, delim))
	{
		strTrim(word);
		splitted.push_back(word);
	}

	return splitted;
}
