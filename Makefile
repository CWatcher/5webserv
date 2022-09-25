.DEFAULT_GOAL  := all
.SECONDEXPANSION:

NAME		= webserv
SRCS		= srcs/main.cpp						\
				srcs/Server.cpp					\
												\
				srcs/config/VirtualServer.cpp	\
				srcs/config/ServerConfig.cpp	\
												\
				srcs/sockets/ASocket.cpp		\
				srcs/sockets/ListenSocket.cpp	\
				srcs/sockets/SessionSocket.cpp	\
												\
				srcs/http/HTTPRequest.cpp		\
				srcs/http/HTTPResponse.cpp		\
												\
				srcs/handlers/AHandler.cpp		\
				srcs/handlers/DeleteHandler.cpp	\
				srcs/handlers/ACgiHandler.cpp	\
				srcs/handlers/GetHandler.cpp	\
				srcs/handlers/PostHandler.cpp	\
				srcs/handlers/PutHandler.cpp	\
												\
				srcs/utils/log.cpp				\
				srcs/utils/string.cpp			\
				srcs/utils/FileInfo.cpp

BDIR       = .objs/
OBJS       = $(SRCS:%.cpp=$(BDIR)%.o)
DFILES     = $(SRCS:%.cpp=$(BDIR)%.d)

CXX			= c++
CPPFLAGS	= -Wall -Wextra -Werror -pedantic -MMD -std=c++98 -Isrcs
debug:		CPPFLAGS += -g3 -fsanitize=address -fsanitize=undefined
debug:		LDFLAGS = -fsanitize=address -fsanitize=undefined
all:		CPPFLAGS += -O2

# ----- Directory create magic --------
.PRECIOUS: $(BDIR)/. $(BDIR)%/.
$(BDIR)/.:
	mkdir -p $@
$(BDIR)%/.:
	mkdir -p $@
# ----- ---------------------- --------

debug:		$(NAME)
all:		$(NAME)

$(BDIR)%.o:	%.cpp | $$(@D)/.
	$(CXX) $(CPPFLAGS) -c $< -o $@ -MD

$(NAME):	$(OBJS) Makefile
	$(CXX) $(LDFLAGS) -o $@ $(OBJS)

$(OBJS):	Makefile

-include $(DFILES)

clean:
	$(RM) $(OBJS) $(DFILES)

fclean:		clean
	$(RM) $(NAME)

re:			fclean all

.PHONY:	all debug clean fclean re
