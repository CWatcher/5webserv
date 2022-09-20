#ifndef SESSIONSOCKET_HPP
# define SESSIONSOCKET_HPP

# include "http/HTTPRequest.hpp"
# include "http/HTTPResponse.hpp"
# include "sockets/ASocket.hpp"

class SessionSocket : public ASocket
{
public:
    explicit        SessionSocket(int fd, in_addr_t from_listen_ip, in_port_t from_listen_port,const in_addr &remote_addr);

    virtual int     action(in_addr &remote_addr);
    void            setStateToWrite();

    const in_addr   &remoteAddr() {return _remote_addr;}
    HTTPRequest     &request() {return _request;}
    HTTPResponse    &response() {return _response;}

private:
    size_t          readRequest();
    size_t          sendResponse();

private:
    static const unsigned   BUFFER_SIZE;

    in_addr          _remote_addr;
    size_t           _written_total;
    HTTPRequest      _request;
    HTTPResponse     _response;
};

#endif
