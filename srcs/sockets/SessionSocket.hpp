#ifndef SESSIONSOCKET_HPP
# define SESSIONSOCKET_HPP

# include "http/HTTPRequest.hpp"
# include "http/HTTPResponse.hpp"
# include "sockets/ASocket.hpp"
# include "handlers/AHandler.hpp"

class SessionSocket : public ASocket
{
public:
    explicit        SessionSocket(int fd, in_addr_t from_listen_ip, in_port_t from_listen_port,const in_addr &remote_addr);
    ~SessionSocket() {delete _handler;}

    virtual int     action(in_addr &remote_addr);
    void            setStateToWrite() {_state = SocketState::Write;}
    void            setHandler(AHandler *handler) {_handler = handler;}

    const in_addr   &remoteAddr() {return _remote_addr;}
    HTTPRequest     &request() {return _request;}
    HTTPResponse    &response() {return _response;}

private:
    void            read();
    void            write();

private:
    in_addr         _remote_addr;
    HTTPRequest     _request;
    HTTPResponse    _response;
    AHandler*       _handler;
};

#endif
