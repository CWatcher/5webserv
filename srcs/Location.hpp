#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "AConfig.hpp"

class Location : public AConfig
{
public:
    Location(const AConfig& config, std::ifstream& f);
    Location() {}

    const std::string&                      path() const {return path_;}
    const std::string&                      directory_page() const {return directory_page_;}
    const std::map<std::string, Location>&  location() const {return location_;}
private:
    void                                    parsePath(std::ifstream& f);
    void                                    parseDirectoryPage(std::ifstream& f);
    void                                    parseLocation(std::ifstream& f);

    void                                    completeConfig(const AConfig& config);
private:
    std::string                             path_;
    std::string                             directory_page_;
    std::map<std::string, Location>         location_;
};

std::ostream&   operator<<(std::ostream& o, const Location& l);

#endif
