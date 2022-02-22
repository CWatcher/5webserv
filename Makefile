include make/tm.mk

PROJECT_NAME  = webserv
NAME          = webserv

CXX           = c++
LD            = c++
CXXFLAGS      = -Wall -Wextra -Werror

${call add/project,${PROJECT_NAME}}
${PROJECT_NAME}_SRCS     += srcs/main.cpp
${PROJECT_NAME}_INC      += ./
${PROJECT_NAME}_CXXFLAGS += --std=c++98
${PROJECT_NAME}_C_DEP    += ./Makefile
${PROJECT_NAME}_LD_DEP   += ./Makefile


${call add/subproj,${PROJECT_NAME},debug}
DEBUG_SANS =    -fsanitize=address \
                -fsanitize=undefined \
                -fstack-protector-all \
                -fstack-check
debug_CXXFLAGS       += -O0 -g3 ${DEBUG_SANS}
debug_LDFLAGS        +=         ${DEBUG_SANS}
${call add/exe,debug,${NAME}}

${call add/subproj,${PROJECT_NAME},thread}
THREAD_SANS =   -fsanitize=thread \
                -fsanitize=undefined \
                -fstack-protector-all \
                -fstack-check
thread_CXXFLAGS       += -O0 -g3 ${THREAD_SANS}
thread_LDFLAGS        +=         ${THREAD_SANS}
${call add/exe,thread,${NAME}}

${call add/subproj,${PROJECT_NAME},release}
release_CXXFLAGS     += -O2
${call add/exe,release,${NAME}}

.PHONY: all clean fclean re

${NAME}: ${debug_EXE}
	cp ${debug_EXE} ./${NAME}
all: ${NAME}
${PROJECT_NAME}_FCLEAN += ${NAME}

clean: debug/clean
fclean: debug/fclean
re: debug/fclean ${NAME}
