#ifndef FILEINFO_HPP
# define FILEINFO_HPP

# include <sys/types.h>
# include <sys/stat.h>
# include <string>

class FileInfo
{
public:
    FileInfo(const std::string &path);
    FileInfo() {}

    bool                isExists() const {return ret_ == 0;}
    off_t               size() const {return stat_.st_size;}
    bool                isDirectory() const {return S_ISDIR(stat_.st_mode);}
    bool                isFile() const {return S_ISREG(stat_.st_mode);}
    bool                isReadble() const {return S_IRUSR & stat_.st_mode;}
    const std::string&  path() const {return path_;}
    std::string         dateStr() const;
    std::string         sizeStr() const;
    std::string         type() const;
private:
    int                 ret_;
    struct ::stat       stat_;
    std::string         path_;
};
#endif

