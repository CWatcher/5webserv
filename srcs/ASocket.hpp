
#ifndef ASOCKET_HPP
# define ASOCKET_HPP

# include "Logger.hpp"

namespace TriggerEvent { enum _ {
    None,
	Read,
	Write
}; }

class ASocket
{
public:
	ASocket(int fd, enum TriggerEvent::_ trigger);
	ASocket(const ASocket &src);
	virtual	~ASocket();

	enum PostAction {
		NoAction,
		Add,
		Process,
		Disconnect
	};

	virtual int     action(enum PostAction &post_action) = 0;
    TriggerEvent::_ getTrigger() const;


private:
	ASocket();
	ASocket	&operator=(const ASocket &rhs);

public:
	const int		fd;

protected:
	TriggerEvent::_	_trigger;
};

#endif
