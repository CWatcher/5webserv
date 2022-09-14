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
												\
				srcs/utils/log.cpp				\
				srcs/utils/string.cpp			\
				srcs/utils/FileInfo.cpp
OBJS		= $(SRCS:.cpp=.o)
CXX			= c++
CPPFLAGS	= -Wall -Wextra -Werror -pedantic -MMD -std=c++98 -Isrcs
debug:		CPPFLAGS += -g3 -fsanitize=address -fsanitize=undefined
debug:		LDFLAGS = -fsanitize=address -fsanitize=undefined
all:		CPPFLAGS += -O2


debug:		$(NAME)

all:		$(NAME)

$(NAME):	$(OBJS) Makefile
	$(CXX) $(LDFLAGS) -o $@ $(OBJS)

$(OBJS):	Makefile

-include $(SRCS:.cpp=.d)

clean:
	$(RM) $(OBJS) $(SRCS:.cpp=.d)

fclean:		clean
	$(RM) $(NAME)

re:			fclean all

.PHONY:	all debug clean fclean re
