#ifndef FILEINFO_HPP
# define FILEINFO_HPP

# include <sys/types.h>
# include <sys/stat.h>
# include <string>
# include <cerrno>

class FileInfo
{
public:
    FileInfo(const std::string &path);
    FileInfo() {}

    bool                isNotExists() const {return ret_ == -1 && error_ == ENOENT;}
    bool                isNoInfo() const {return ret_ == -1;}
    bool                isInfoValid() const {return ret_ == 0;}
    bool                isDirectory() const {return S_ISDIR(stat_.st_mode);}
    bool                isFile() const {return S_ISREG(stat_.st_mode);}
    bool                isReadble() const {return S_IRUSR & stat_.st_mode;}

    std::string         dateStr() const;
    off_t               size() const {return stat_.st_size;}
    std::string         sizeStr() const;

    const std::string&  path() const {return path_;}
    std::string         type() const;
private:
    int                 ret_;
    int                 error_;
    struct ::stat       stat_;
    std::string         path_;
};
#endif

