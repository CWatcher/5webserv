#ifndef SOCKETSESSION_HPP
# define SOCKETSESSION_HPP

# include "http/HTTPRequest.hpp"
# include "http/HTTPResponse.hpp"
# include "socket/ASocket.hpp"

class SocketSession : public ASocket
{
public:
    explicit            SocketSession(int fd, in_addr_t from_listen_ip, in_port_t from_listen_port, sockaddr_in &remote_addr);

    virtual int         action(sockaddr_in &remote_addr);
    void                setStateToWrite();

    const sockaddr_in   &remote() {return _remote;}
    HTTPRequest         &request() {return _request;}
    HTTPResponse        &response() {return _response;}

private:
    size_t              actionRead();
    size_t              actionWrite();

private:
    sockaddr_in         _remote;
    size_t              _written_total;
    HTTPRequest         _request;
    HTTPResponse        _response;
};

#endif
