include make/tm.mk

PROJECT_NAME  = webserv
NAME          = webserv

CXX           = c++
LD            = c++
CXXFLAGS      = -Wall -Wextra -Werror --std=c++98
DEP           = ./Makefile

BUILD        ?= debug

$(call add/project,$(PROJECT_NAME))
$(PROJECT_NAME)_SRCS     += srcs/main.cpp

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
	cp $($(BUILD)_EXE) $(NAME)
all: $(NAME)
$(BUILD)_BUILD  += $(NAME)
$(BUILD)_FCLEAN += $(NAME)

clean: debug/clean release/clean
fclean: debug/fclean release/fclean
re: $(BUILD)/re
