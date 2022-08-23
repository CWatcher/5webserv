#include "utils/FileInfo.hpp"

#include <sstream>

FileInfo::FileInfo(const std::string &path) : ret_(::stat(path.c_str(), &stat_)),  path_(path)
{
    if (isDirectory() && *--path_.end() != '/')
        path_.push_back('/');
}

std::string FileInfo::dateStr() const
{
    char    date[21];

    ::strftime(date, 21, "%d-%b-%Y %H:%M", ::gmtime(&stat_.st_mtim.tv_sec));
    return date;
}

std::string FileInfo::sizeStr() const
{
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