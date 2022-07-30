
#ifndef HTTPMESSAGE_HPP
# define HTTPMESSAGE_HPP

# include "utils/log.hpp"
# include "utils/string.hpp"

# include <map>
# include <string>

class HTTPMessage
{
public:
    HTTPMessage();
    HTTPMessage(const HTTPMessage &src);
    ~HTTPMessage();
    HTTPMessage	&operator=(const HTTPMessage &rhs);
    void		operator+=(const std::string &rhs);

    bool								hasEndOfMessage();
    std::string							*getHeaderValue(const std::string &header_key);
    std::string							getHeaderHostName();

private:
    void								parseHeader();
    void                                parseStartingLine();
    std::map<std::string, std::string>	getHeaderMapFromRaw();
    std::pair<std::string, std::string>	getHeaderPairFromLine(const std::string &line);

public:
    std::string							raw_data;
    std::string                         starting_line;
    std::string                         method;
    std::string                         url;
    std::map<std::string, std::string>	header;

private:
    size_t	_header_size;
    size_t	_body_size;
};


#endif
