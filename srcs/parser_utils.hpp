#ifndef PARSER_UTILS
# define PARSER_UTILS

# include <string>
# include <fstream>
# include <vector>

void	parse_single_str_value(std::ifstream& f, std::string& value);
void	parse_uint_value(std::ifstream& f, unsigned& value);
void	parse_autoindex(std::ifstream& f, bool& value);
void	parse_index(std::ifstream& f, std::vector<std::string>& value);
// void	parse_method();

#endif
