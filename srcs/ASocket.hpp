
#ifndef ASOCKET_HPP
# define ASOCKET_HPP

# include "Logger.hpp"

namespace TriggerType { enum _ {
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
	void		disconnect() const;


private:
	ASocket();
	ASocket	&operator=(const ASocket &rhs);

public:
	const int				fd;
	const TriggerType::_	trigger;
};

#endif
