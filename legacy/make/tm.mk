.DEFAULT_GOAL  := all
.SECONDEXPANSION:
# Variable cheat-sheet

# PYTHON - for python scripts
# NORMINETTE - norminette runner
# RM - remove programm with flags

# CC - c-lang compiler
# CFLAGS - c-lang flags
# CXX - cpp-lang compiler
# CXXFLAGS - c-lang flags
# INC - include directories (without '-I' flag)

# LD - linker
# LDFLAGS - linkflags
# AR - lib archivator
# ARFLAGS - lib archivator flags

# BUILD - all build targets
# EXE - executable (unset if project dont 'add/exe')
# LIB - library (unset if project dont 'add/lib')
# SHLIB - shared library (unset if project dont 'add/shlib')

# CLEAN - what need remove with 'clean' command
# FCLEAN - what need remove with 'fclean' command

# External programms
PYTHON         ?= python3
NORMINETTE     ?= norminette

# Pretty print
COLORED       ?= 1
ifeq ($(COLORED), 1)
color_RED      = \033[31m
color_GREEN    = \033[32m
color_YELLOW   = \033[33m
color_BLUE     = \033[34m
color_MAGENTA  = \033[35m
color_CYAN     = \033[36m
color_RESET    = \033[0m
reset_line     = \033[A\033[2K
endif

pretty_work     = $(color_YELLOW)⟳$(color_RESET)
pretty_done     = $(color_GREEN)✓$(color_RESET)
pretty_fail     = $(color_RED)✕$(color_RESET)
pretty_clean    = $(color_YELLOW)♻$(color_RESET)
pretty_file     = $(color_CYAN)%s$(color_RESET)

define __print = 
@printf "[$(1)][$(pretty_file)] %s\n"  $(2) $(3)
endef

define __in_work = 
$(call __print,$(pretty_work),$(1),$(2))
endef

define __done = 
$(call __print,$(pretty_done),$(1),$(2))
endef

define __clean = 
$(call __print,$(pretty_clean),$(1),$(2))
endef

empty           =
space           = $(empty) $(empty)
comma           = $(empty),$(empty)
silent          = $(empty)@$(empty)
force_ignore_error = 2>/dev/null || true

ifeq ($(VERBOSE),1)
silent          = $(empty)
endif

__ALL_DFILES =

#######################################################
#
# Real build
#

TARGET_OS = $(shell uname)

define __add/project =
$(1)_CC         = $(CC)
$(1)_CXX        = $(CXX)
$(1)_INC        = $(INC)
$(1)_FINC       = $$($(1)_INC:%=-I%)
$(1)_LD         = $(LD)
$(1)_LIBS       = $(LIBS)
$(1)_AR         = $(AR)

$(1)_CFLAGS     = $(CFLAGS)
$(1)_CXXFLAGS   = $(CXXFLAGS)
$(1)_LDFLAGS    = $(LDFLAGS)
$(1)_ARFLAGS    = $(ARFLAGS)

$(1)_BUILD      =
$(1)_SRCS       =
$(1)_BDIR       = .$(1)/
$(1)_OBJS       = $$($(1)_SRCS:%=$$($(1)_BDIR)%.o)
$(1)_DFILES     = $$($(1)_SRCS:%=$$($(1)_BDIR)%.d)
__ALL_DFILES   += $$($(1)_DFILES)

$(1)_CLEAN      =
$(1)_FCLEAN     =

$(1)_DEP        = $(DEP)
$(1)_C_DEP      = $$($(1)_DEP)
$(1)_CXX_DEP    = $$($(1)_DEP)
$(1)_AR_DEP     = $$($(1)_DEP)
$(1)_LD_DEP     = $$($(1)_DEP)

$(1)_CLEAN_DEP  =
$(1)_FCLEAN_DEP = $(1)/clean

$(call __add/build_dir,$(1))
$(call __add/objrule,$(1))
$(call __add/baserules,$(1))
endef

define __add/subproj =
$(call __add/project,$(2))
$(2)_CC         = $$($(1)_CC)
$(2)_CXX        = $$($(1)_CXX)
$(2)_INC        = $$($(1)_INC)
$(2)_LD         = $$($(1)_LD)
$(2)_LIBS       = $$($(1)_LIBS)
$(2)_AR         = $$($(1)_AR)

$(2)_CFLAGS     = $$($(1)_CFLAGS)
$(2)_CXXFLAGS   = $$($(1)_CXXFLAGS)
$(2)_LDFLAGS    = $$($(1)_LDFLAGS)
$(2)_ARFLAGS    = $$($(1)_ARFLAGS)

$(2)_SRCS       = $$($(1)_SRCS)

$(2)_DEP        = $$($(1)_DEP)
$(2)_C_DEP      = $$($(1)_C_DEP)
$(2)_CXX_DEP    = $$($(1)_CXX_DEP)
$(2)_LD_DEP     = $$($(1)_LD_DEP)
$(2)_AR_DEP     = $$($(1)_AR_DEP)
endef

# auto build directories
# root src files must start with "./" ... or mb not
define __add/build_dir =
.PRECIOUS: $$($(1)_BDIR)/. $$($(1)_BDIR)%/.
$$($(1)_BDIR)/.:
	$(silent)mkdir -p $$@
$$($(1)_BDIR)%/.:
	$(silent)mkdir -p $$@
endef

define __add/objrule =
$$($(1)_BDIR)%.cpp.o:	%.cpp $$($(1)_CXX_DEP) | $$$$(@D)/.
	$(call __in_work,$$@,compile)
	$(silent)$$($(1)_CXX) $$($(1)_CXXFLAGS) $$($(1)_FINC)  -c $$< -o $$@ -MD
	$(call __done,$$@,compile)

$$($(1)_BDIR)%.c.o:	%.c $$($(1)_C_DEP) | $$$$(@D)/.
	$(call __in_work,$$@,compile)
	$(silent)$$($(1)_CC) $$($(1)_CFLAGS) $$($(1)_FINC)  -c $$< -o $$@ -MD
	$(call __done,$$@,compile)
endef

define __add/baserules =
.PHONY: $(1)/clean
$(1)/clean: $$($(1)_CLEAN_DEP)
	-$(silent)$(RM) $$($(1)_CLEAN) $$($(1)_OBJS) $$($(1)_DFILES)
	$(call __clean,$$@)

.PHONY: $(1)/fclean
$(1)/fclean: $$($(1)_FCLEAN_DEP)
	-$(silent)$(RM) $$($(1)_FCLEAN) $$($(1)_BUILD)
	-$(silent)find $$($(1)_BDIR) -type d -empty -delete $(force_ignore_error)
	$(call __clean,$$@)

.PHONY: $(1)/re
$(1)/re: $(1)/fclean
	+$(silent)$(MAKE) $$($(1)_BUILD)
endef


#######################################################
#
# Build targets
#

#! @1 - project name
#! @2 - out name
define __add/exe =
$(1)_EXE = $$($(1)_BDIR)$(2)
$(1)_BUILD += $$($(1)_EXE)

$$($(1)_EXE): $$($(1)_OBJS) $$($(1)_LD_DEP)
	$(call __in_work,$$@,linking)
	$(silent)$$($(1)_LD) $$($(1)_LDFLAGS) $$($(1)_OBJS) -o $$($(1)_EXE) $$($(1)_LIBS)
	$(call __done,$$@,linking)

.PHONY: $(1)/exe
$(1)/exe: $$($(1)_EXE)
endef

#! @1 - project name
#! @2 - out name
define __add/lib =
$(1)_LIB = $$($(1)_BDIR)/$(2)
$(1)_BUILD += $$($(1)_LIB)

$$($(1)_LIB): $$($(1)_OBJS) $$($(1)_AR_DEP)
	$(call __in_work,$$@,archiving)
	$(silent)$$($(1)_AR) $$($(1)_ARFLAGS) $$($(1)_LIB) $$($(1)_OBJS)
	$(call __done,$$@,archiving)

.PHONY: $(1)/lib
$(1)/lib: $$($(1)_LIB)
endef

#! @1 - project name
#! @2 - out name
define __add/shlib =
$(1)_SHLIB = $$($(1)_BDIR)/$(2)
$(1)_BUILD += $$($(1)_SHLIB)

$$($(1)_SHLIB): $$($(1)_OBJS) $$($(1)_LD_DEP)
	$(call __in_work,$$@,linking)
	$(silent)$$($(1)_LD) $$($(1)_LDFLAGS) -shared -Wl,-soname,$(2) -o $$($(1)_SHLIB) $$($(1)_OBJS) $$($(1)_LIBS)
	$(call __in_work,$$@,linking)

.PHONY: $(1)/shlib
$(1)/shlib: $$($(1)_SHLIB)
endef

define __add/header_dep = 
-include $$(__ALL_DFILES)
endef

#! @1 - project name
add/project = $(eval $(call __add/project,$(1),$(2)))

#! @1 - project name
#! @2 - sub project name
add/subproj = $(eval $(call __add/subproj,$(1),$(2)))

#! @1 - project name
#! @2 - out name
add/exe = $(eval $(call __add/exe,$(1),$(2)))
add/lib = $(eval $(call __add/lib,$(1),$(2)))
add/shlib = $(eval $(call __add/shlib,$(1),$(2)))

#! must be called after all .c/.cpp file addition
add/header_dep = $(eval $(call __add/header_dep))
