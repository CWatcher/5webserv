#include "utils/FileInfo.hpp"

#include <sstream>

FileInfo::FileInfo(const std::string &path) : ret_(::stat(path.c_str(), &stat_)), error_(errno), path_(path)
{
    if (ret_ == 0 && isDirectory() && *path_.rbegin() != '/')
        path_.push_back('/');
}

std::string FileInfo::dateStr() const
{
    char    date[21];

    if (ret_ == -1)
        return "?";
#ifdef __APPLE__
//TO_TEST
    ::strftime(date, 21, "%d-%b-%Y %H:%M", ::gmtime(&stat_.st_mtimespec.tv_sec));
#else
    ::strftime(date, 21, "%d-%b-%Y %H:%M", ::gmtime(&stat_.st_mtim.tv_sec));
#endif
    return date;
}

std::string FileInfo::sizeStr() const
{
    if (ret_ == -1)
        return "?";
    if (isFile())
    {
        std::stringstream   ss;
        ss << size();
        return ss.str();
    }
    return "-";
}

std::string FileInfo::type() const
{
    size_t  type = path_.rfind('.');

    if (type == std::string::npos)
        return "";
    return path_.substr(type).erase(0, 1);
}
