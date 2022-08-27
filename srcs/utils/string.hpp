
#ifndef STRING_HPP
# define STRING_HPP

# include <string>
# include <vector>

std::string					&strTrim(std::string &str);
std::string					&strLowerCase(std::string &str);
std::string					strLowerCaseCopy(const std::string &str);
std::string					&strRemoveDoubled(std::string &str, char c);
std::string					&strCompleteWith(std::string &str, char c);
bool						strEndsWith(const std::string &str, const std::string &suffix);
std::vector<std::string>	strSplit(const std::string &str, char delim);

#endif
