include make/tm.mk

PROJECT_NAME  = webserv
SRCS          = main.cpp
NAME          = webserv
BUILD_DIR     = .build/

LD            = g++
CXXFLAGS      = -Wall -Wextra -Werror

${call add/project,${PROJECT_NAME}}
${PROJECT_NAME}_SRCS += ./main.cpp

${call add/subproj,${PROJECT_NAME},debug}
debug_CXXFLAGS += -O0 -g3
${call add/exe,debug,${NAME}}

.PHONY: all clean fclean re
all: webserv/debug/exe
clean: webserv/debug/clean
fclean: webserv/debug/fclean
re: webserv/debug/fclean webserv/debug/exe
