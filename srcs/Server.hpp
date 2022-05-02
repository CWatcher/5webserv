#ifndef SERVER_HPP
# define SERVER_HPP

# include <string> //
# include <vector> //
# include <fstream> //
# include <map> //
# include <netinet/in.h>
# include <arpa/inet.h>

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

class   Server 
{
public:
    Server();

    void                                    parseConfig(std::ifstream& file);

    const std::string&                      host() const  {return host_;}
    const std::string&                      port() const  {return port_;}
    const sockaddr_in&                      host_port() const {return host_port_;}
    const std::string&                      server_name() const  {return server_name_;}
    const std::string&                      root() const  {return root_;}
    const std::vector<std::string>          index() const  {return index_;}
    const std::string&                      error_page() const  {return error_page_;}
    unsigned                                body_size() const  {return body_size_;}
    bool                                    autoindex() const {return autoindex_;}
    const std::map<std::string, Location>&  location() const {return location_;}
    // const std::map<std::string, Cgi>&       cgi() const {return cgi_;}
private:
    void                                    parseListen(std::ifstream& f);
    void                                    parseServerName(std::ifstream& f);
    void                                    parseRoot(std::ifstream& f);
    void                                    parseIndex(std::ifstream& f);
    void                                    parseErrorPage(std::ifstream& f);
    void                                    parseBodySize(std::ifstream& f);
    // void                                    parseMethods();
    // void                                    parseLocation(std::ifstream& f);
    // void                                    parseCgi(std::ifstream& f, Server* s);
private:
    std::string                             host_;
    std::string                             port_;
    sockaddr_in                             host_port_;
    std::string                             server_name_;
    std::string                             root_;
    std::vector<std::string>                index_;
    std::string                             error_page_;
    unsigned                                body_size_;
    bool                                    autoindex_;
    //char                                  methods_;
    std::map<std::string, Location>         location_;
    // std::map<std::string, Cgi>                      cgi_; ??

    typedef void                            (Server::*f)(std::ifstream& file);
    static const std::pair<std::string, f>  parser_init_list_[]; 
    static const std::map<std::string, f>   parser;
};

#endif
