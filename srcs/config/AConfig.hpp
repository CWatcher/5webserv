#ifndef ACONFIG_HPP
# define ACONFIG_HPP

# include <string>
# include <vector>
# include <set>
# include <map>
# include <fstream>

class AConfig
{
public:
    AConfig ();
    virtual ~AConfig() = 0;

    const std::string&                  root() const  {return root_;}
    const std::vector<std::string>&     index() const  {return index_;}
    bool                                autoindex() const {return autoindex_;}
    const std::map<int, std::string>&   error_page() const {return error_page_;}
    unsigned                            body_size() const {return body_size_;}
    const std::set<std::string>&        methods() const {return methods_;}
protected:
    void                                parseRoot(std::ifstream& f);
    void                                parseIndex(std::ifstream& f);
    void                                parseAutoindex(std::ifstream& f);
    void                                parseErrorPage(std::ifstream& f);
    void                                parseBodySize(std::ifstream& f);
    void                                parseMethods(std::ifstream& f);

    void                                completeConfig(const AConfig& config);
private:
    std::string                         root_;
    std::vector<std::string>            index_;
    int                                 autoindex_;
    std::map<int, std::string>          error_page_;
    unsigned                            body_size_;
    std::set<std::string>               methods_;
};

std::ostream&   operator<<(std::ostream& o, const AConfig& c);

# endif
