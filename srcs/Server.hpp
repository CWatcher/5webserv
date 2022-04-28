#ifndef SERVER_HPP
# define SERVER_HPP

# include <string> //
# include <vector> //
# include <fstream> //
# include <map> //
# include <netinet/in.h>

struct  Location
{
    char    metods;
    //redirect?
    bool    autoindex;
    //default file if directory?
    // cgi?
};


struct  Cgi
{
    /* data */
    // Location ??
};


#define SPACES " \f\n\r\t\v"

class   Server 
{
public:
    Server();

    void                                    readConfig(std::ifstream& file);

    const std::string&                      host_str() const  {return host_str_;}
    in_addr_t                               port() const  {return port_;}
    const std::string&                      server_name() const  {return server_name_;}
    const std::string&                      root() const  {return root_;}
    const std::vector<std::string>          index() const  {return index_;}
    const std::string&                      error_page() const  {return error_page_;}
    unsigned                                body_size() const  {return body_size_;}
    const std::map<std::string, Location>&  location() const {return location_;}
    const std::map<std::string, Cgi>&       cgi() const {return cgi_;}
private:
    void                                    parseListen(std::ifstream& f);
    void                                    parseServerName(std::ifstream& f);
    void                                    parseRoot(std::ifstream& f);
    void                                    parseIndex(std::ifstream& f);
    void                                    parseErrorPage(std::ifstream& f);
    void                                    parseBodySize(std::ifstream& f);
    // void                                    parseLocation(std::ifstream& f, Server* s);
    // void                                    parseCgi(std::ifstream& f, Server* s);
public:
    typedef void                            (Server::*f)(std::ifstream& file);
    static const std::map<std::string, f>   parser;
private:
    std::string                                     host_str_;
    // ???                                          host;
    in_addr_t                                       port_;
    std::string                                     server_name_;
    std::string                                     root_;
    std::vector<std::string>                        index_;
    std::string                                     error_page_;
    unsigned                                        body_size_;
    std::map<std::string, Location>                 location_;
    std::map<std::string, Cgi>                      cgi_;

    static const std::pair<std::string, Server::f>  parser_init_list_[]; 
};

#endif
