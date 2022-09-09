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
				srcs/old_handlers/runner/runner.cpp		\
				srcs/old_handlers/base/Handler.cpp		\
				srcs/old_handlers/CGIReader.cpp			\
				srcs/old_handlers/FileReader.cpp		\
				srcs/old_handlers/HeaderGenerator.cpp	\
				srcs/old_handlers/HeaderParser.cpp		\
				srcs/old_handlers/HeaderValidator.cpp	\
												\
				srcs/utils/log.cpp				\
				srcs/utils/string.cpp			\
				srcs/utils/FileInfo.cpp

OBJS		= $(SRCS:.cpp=.o)
#CXX			= g++
CPPFLAGS	= -Wall -Wextra -Werror -MMD -std=c++98 -Isrcs

all:		$(NAME)

$(NAME):	$(OBJS)
	 $(CXX) -o $(NAME) $(OBJS)

-include $(SRCS:.cpp=.d)

clean:
	 $(RM) $(OBJS) $(SRCS:.cpp=.d)

fclean:		clean
	 $(RM) $(NAME)

re:	 fclean all

.PHONY:	all clean fclean re
