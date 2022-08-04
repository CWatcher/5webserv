#ifndef STAT_HPP
# define STAT_HPP

# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include <string>

namespace ft
{
class stat
{
public:
	stat(const std::string &file) {_ret = ::stat(file.c_str(), &_stat);}
	bool	isExists() const {return _ret == 0;}
	off_t	size() const {return _stat.st_size;}
	bool	isDirectory() const {return S_ISDIR(_stat.st_mode);}
	bool	isFile() const {return S_ISREG(_stat.st_mode);}
	bool	isReadble() const {return S_IRUSR & _stat.st_mode;}
private:
	int				_ret;
	struct ::stat	_stat;
};
}
#endif

