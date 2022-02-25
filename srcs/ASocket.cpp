
#include "ASocket.hpp"

#include <unistd.h>

ASocket::ASocket(int fd, enum TriggerEvent::_ trigger)
	: fd(fd)
	, _trigger(trigger) {}

ASocket::ASocket(const ASocket &src)
	: fd(src.fd)
	, _trigger(src._trigger) {}

ASocket::~ASocket()
{
    close(fd);
    logger::info("Disconnected socket", fd);
}

TriggerEvent::_ ASocket::getTrigger() const
{
    return _trigger;
}

ASocket::ASocket() : fd(0), _trigger(TriggerEvent::Read) {}
ASocket	&ASocket::operator=(const ASocket &_) {(void)_; return *this;}
