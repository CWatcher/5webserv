#include <algorithm>
#include <string>

std::string	&strTrim(std::string &str)
{
	const size_t	startpos = str.find_first_not_of(" \t\v\r\n");
	const size_t	endpos = str.find_last_not_of(" \t\v\r\n");

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
	if (*--str.end() != c)
		str.push_back(c);
	return str;
}
