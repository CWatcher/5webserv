include make/tm.mk

PROJECT_NAME  = webserv
NAME          = webserv

CXX           = g++
LD            = g++
CXXFLAGS      = -Wall -Wextra -Werror

${call add/project,${PROJECT_NAME}}
${PROJECT_NAME}_SRCS     += ./main.cpp
${PROJECT_NAME}_INC      += ./
${PROJECT_NAME}_CXXFLAGS += --std=c++98

${call add/subproj,${PROJECT_NAME},debug}
debug_CXXFLAGS       += -O0 -g3
${call add/exe,debug,${NAME}}

${call add/subproj,${PROJECT_NAME},release}
release_CXXFLAGS     += -O2
${call add/exe,release,${NAME}}

.PHONY: all clean fclean re

${NAME}: debug/exe
	cp ${debug_BDIR}${NAME} ./${NAME}
all: ${NAME}
${PROJECT_NAME}_FCLEAN += ${NAME}

clean: debug/clean
fclean: debug/fclean
re: debug/fclean debug/exe
