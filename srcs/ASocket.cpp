
#include "ASocket.hpp"

#include <unistd.h>

ASocket::ASocket(int fd, enum TriggerType::_ trigger)
	: fd(fd)
	, trigger(trigger) {}

ASocket::ASocket(const ASocket &src)
	: fd(src.fd)
	, trigger(src.trigger) {}

ASocket::~ASocket() {}

void	ASocket::disconnect() const
{
	close(fd);
	log::info("Disconnected socket", fd);
}

ASocket::ASocket() : fd(0), trigger(TriggerType::Read) {}
ASocket	&ASocket::operator=(const ASocket &_) {(void)_; return *this;}
