#include "utils/stat.hpp"

#include <sstream>

std::string	ft::stat::strDate() const
{
    char    date[21];

    ::strftime(date, 21, "%d-%b-%Y %H:%M", ::gmtime(&stat_.st_mtim.tv_sec));
    return date;
}

std::string	ft::stat::strSize() const
{
    if (isFile())
    {
        std::stringstream   ss;
        ss << size();
        return ss.str();
    }
    return "-";
}
