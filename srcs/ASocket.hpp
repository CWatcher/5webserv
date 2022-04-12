
#ifndef ASOCKET_HPP
# define ASOCKET_HPP

# include "Logger.hpp"

namespace TriggerType { enum _ {
    None,
	Read,
	Write
}; }

class ASocket
{
public:
	ASocket(int fd, enum TriggerType::_ trigger);
	ASocket(const ASocket &src);
	virtual	~ASocket();

	enum PostAction {
		NoAction,
		Add,
		Process,
		Read,
		Disconnect
	};

	virtual int	action(enum PostAction &post_action) = 0;

    TriggerType::_ getTrigger() const;


private:
	ASocket();
	ASocket	&operator=(const ASocket &rhs);

public:
	const int		fd;

protected:
	TriggerType::_	_trigger;
};

#endif
