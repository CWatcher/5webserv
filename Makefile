include make/tm.mk

PROJECT_NAME  = webserv
NAME          = webserv

CXX           = c++
LD            = c++
CXXFLAGS      = -Wall -Wextra -Werror

BUILD ?= debug

$(call add/project,$(PROJECT_NAME))
$(PROJECT_NAME)_SRCS     += srcs/main.cpp			\
							srcs/Logger.cpp			\
							srcs/Server.cpp			\
							srcs/ASocket.cpp		\
							srcs/SocketListen.cpp	\
							srcs/SocketSession.cpp	\
							srcs/HTTPMessage.cpp	\
							srcs/utils.cpp

$(PROJECT_NAME)_INC      += ./
$(PROJECT_NAME)_CXXFLAGS += --std=c++98
$(PROJECT_NAME)_C_DEP    += ./Makefile
$(PROJECT_NAME)_LD_DEP   += ./Makefile


$(call add/subproj,$(PROJECT_NAME),debug)
DEBUG_SANS =    -fsanitize=address \
                -fsanitize=undefined \
                -fstack-protector-all \
                -fstack-check
debug_CXXFLAGS       += -O0 -g3 $(DEBUG_SANS)
debug_LDFLAGS        +=         $(DEBUG_SANS)
$(call add/exe,debug,$(NAME))

$(call add/subproj,$(PROJECT_NAME),thread)
THREAD_SANS =   -fsanitize=thread \
                -fsanitize=undefined \
                -fstack-protector-all \
                -fstack-check
thread_CXXFLAGS       += -O0 -g3 $(THREAD_SANS)
thread_LDFLAGS        +=         $(THREAD_SANS)
$(call add/exe,thread,$(NAME))

$(call add/subproj,$(PROJECT_NAME),release)
release_CXXFLAGS     += -O2
$(call add/exe,release,$(NAME))

.PHONY: all clean fclean re

$(NAME): $($(BUILD)_EXE)
	cp $($(BUILD)_EXE) $(NAME)
all: $(NAME)
$(BUILD)_BUILD  += $(NAME)
$(BUILD)_FCLEAN += $(NAME)

clean: $(BUILD)/clean
fclean: $(BUILD)/fclean
re: $(BUILD)/re
