include make/tm.mk

PROJECT_NAME  = webserv
NAME          = webserv

CXX           = c++
LD            = c++
CXXFLAGS      = -Wall -Wextra -Werror --std=c++98 -Isrcs
LDFLAGS       = -pthread
DEP           = ./Makefile

BUILD        ?= debug


$(call add/project,$(PROJECT_NAME))
$(PROJECT_NAME)_SRCS     += srcs/main.cpp			\
							srcs/Server.cpp			\
							srcs/ASocket.cpp		\
							srcs/SocketListen.cpp	\
							srcs/SocketSession.cpp	\
							srcs/ThreadPool.cpp		\
							srcs/HTTPMessage.cpp	\
							srcs/utils/log.cpp		\
							srcs/utils/string.cpp	\
																\
							srcs/handlers/starter.cpp			\
							srcs/handlers/base/Handler.cpp		\
							srcs/handlers/CGIReader.cpp			\
							srcs/handlers/FileReader.cpp		\
							srcs/handlers/HeaderGenerator.cpp	\
							srcs/handlers/HeaderParser.cpp		\
							srcs/handlers/HeaderValidator.cpp

$(call add/subproj,$(PROJECT_NAME),debugbase)
debugbase_CXXFLAGS       += -O0 -g3

$(call add/subproj,debugbase,debug)
DEBUG_SANS =    -fsanitize=address \
                -fsanitize=undefined
debug_CXXFLAGS       += $(DEBUG_SANS)
debug_LDFLAGS        += $(DEBUG_SANS)
$(call add/exe,debug,$(NAME))

$(call add/subproj,$(PROJECT_NAME),release)
release_CXXFLAGS     += -O2
$(call add/exe,release,$(NAME))

.PHONY: all clean fclean re

$(NAME): $($(BUILD)_EXE)
	$(silent)cp $($(BUILD)_EXE) $(NAME)
all: $(NAME)
$(BUILD)_BUILD  += $(NAME)
$(BUILD)_FCLEAN += $(NAME)

clean: debug/clean release/clean
fclean: debug/fclean release/fclean
re: $(BUILD)/re

$(call add/header_dep)
