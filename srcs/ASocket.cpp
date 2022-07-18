
#include "ASocket.hpp"

#include <unistd.h>

ASocket::ASocket(int fd, enum TriggerType::_ trigger)
	: fd(fd)
	, _trigger(trigger) {}

ASocket::ASocket(const ASocket &src)
	: fd(src.fd)
	, _trigger(src._trigger) {}

ASocket::~ASocket()
{
    close(fd);
    logger::info << "Disconnected socket " << fd << logger::end;
}

TriggerType::_ ASocket::getTrigger() const
{
    return _trigger;
}
