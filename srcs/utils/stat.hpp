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
	stat(const std::string &file) {ret_ = ::stat(file.c_str(), &stat_);}
	bool		isExists() const {return ret_ == 0;}
	off_t		size() const {return stat_.st_size;}
	bool		isDirectory() const {return S_ISDIR(stat_.st_mode);}
	bool		isFile() const {return S_ISREG(stat_.st_mode);}
	bool		isReadble() const {return S_IRUSR & stat_.st_mode;}
	std::string	strDate() const;
	std::string	strSize() const;
private:
	int				ret_;
	struct ::stat	stat_;
};
}
#endif

