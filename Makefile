include make/tm.mk

PROJECT_NAME  = webserv
NAME          = webserv

CXX           = c++
LD            = c++
CXXFLAGS      = -Wall -Wextra -Werror -pedantic -std=c++98
LDFLAGS       =
DEP           = ./Makefile
INC           = ./srcs

BUILD        ?= release

$(call add/project,$(PROJECT_NAME))
$(PROJECT_NAME)_SRCS =	srcs/main.cpp						\
						srcs/Server.cpp						\
															\
						srcs/config/VirtualServer.cpp		\
						srcs/config/ServerConfig.cpp		\
															\
						srcs/sockets/ASocket.cpp			\
						srcs/sockets/ListenSocket.cpp		\
						srcs/sockets/SessionSocket.cpp		\
															\
						srcs/http/HTTPRequest.cpp			\
						srcs/http/HTTPResponse.cpp			\
															\
						srcs/handlers/AHandler.cpp			\
						srcs/handlers/DeleteHandler.cpp		\
						srcs/handlers/ACgiHandler.cpp		\
						srcs/handlers/GetHandler.cpp		\
						srcs/handlers/PostHandler.cpp		\
						srcs/handlers/PutHandler.cpp		\
						srcs/handlers/UndefinedHandler.cpp	\
															\
						srcs/utils/log.cpp					\
						srcs/utils/string.cpp				\
						srcs/utils/FileInfo.cpp

$(call add/subproj,$(PROJECT_NAME),debugbase)
debugbase_CXXFLAGS       += -O0 -g3 -DLOG_LEVEL=\"debug\"

# ------- debug build ---------
$(call add/subproj,debugbase,debug)
DEBUG_SANS =    -fsanitize=address \
                -fsanitize=undefined
debug_CXXFLAGS       += $(DEBUG_SANS)
debug_LDFLAGS        += $(DEBUG_SANS)
$(call add/exe,debug,$(NAME))
# ------- ----------- ---------

# ------- release build ---------
$(call add/subproj,$(PROJECT_NAME),release)
release_CXXFLAGS     += -O2 -DLOG_LEVEL=\"info\"
$(call add/exe,release,$(NAME))
# ------- ------------- ---------

.PHONY: all clean fclean re

$(NAME): $($(BUILD)_EXE)
	$(silent)cp $($(BUILD)_EXE) $(NAME)
all: $(NAME)
$(BUILD)_BUILD  += $(NAME)
$(BUILD)_FCLEAN += $(NAME)

clean_exe:
	-$(silent)$(RM) $(NAME)

clean: debug/clean release/clean
fclean: debug/fclean release/fclean
re: $(BUILD)/re

$(call add/header_dep)
