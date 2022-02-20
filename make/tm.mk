.DEFAULT_GOAL  := all
.SECONDEXPANSION:
# External programms
PYTHON          = python3
NORMINETTE      = norminette
RM              = rm -f
AR              = ar rcs
MKDIR           = mkdir -p

# Pretty print
colored        = 1
ifeq (${colored}, 1)
color_RED      = \x1b[31m
color_GREEN    = \x1b[32m
color_YELLOW   = \x1b[33m
color_BLUE     = \x1b[34m
color_MAGENTA  = \x1b[35m
color_CYAN     = \x1b[36m
color_RESET    = \x1b[0m
reset_line     = \033[A\033[2K
endif

pretty_done     = [${color_GREEN}✓${color_RESET}]
pretty_fail     = [${color_RED}✕${color_RESET}]

empty           =
space           = ${empty} ${empty}
comma           = ${empty},${empty}

#######################################################
#
# Real build
#

TARGET_OS = ${shell uname}
#ifeq (${TARGET_OS}, Linux)
#endif
#ifeq (${TARGET_OS}, Darwin)
#endif

define __add/project =
$(1)_CC         = $(CC)
$(1)_CXX        = $(CXX)
$(1)_CFLAGS     = $(CFLAGS)
$(1)_CXXFLAGS   = $(CXXFLAGS)
$(1)_INC        =
$(1)_FINC       = $${$(1)_INC:%=-I%}
$(1)_LD         = $(LD)
$(1)_LDFLAGS    = $(LDFLAGS)
$(1)_LIBS       = $(LIBS)

$(1)_C_DEP      =
$(1)_LD_DEP     =
$(1)_CLEAN_DEP  =
$(1)_FCLEAN_DEP =

$(1)_SRCS       =
$(1)_BDIR       = .$(1)/
$(1)_OBJS       = $${$(1)_SRCS:%.cpp=$${$(1)_BDIR}%.o}
$(1)_DFILES     = $${$(1)_SRCS:%.cpp=$${$(1)_BDIR}%.d}
-include $${$(1)_DFILES}

$(1)_CLEAN      = 
$(1)_FCLEAN     = $$($(1)_CLEAN)

$(call __add/build_dir,${1})
$(call __add/objrule,${1})
$(call __add/baserules,${1})
endef

define __add/subproj =
$(call __add/project,$(2))
$(2)_CC        = $$($(1)_CC)
$(2)_CXX       = $$($(1)_CXX)
$(2)_CFLAGS    = $$($(1)_CFLAGS)
$(2)_CXXFLAGS  = $$($(1)_CXXFLAGS)
$(2)_INC       = $$($(1)_INC)
$(2)_LD        = $$($(1)_LD)
$(2)_LDFLAGS   = $$($(1)_LDFLAGS)
$(2)_LIBS      = $$($(1)_LIBS)
$(2)_SRCS      = $$($(1)_SRCS)

$(2)_C_DEP      = $$($(1)_C_DEP)
$(2)_LD_DEP     = $$($(1)_LD_DEP)
$(2)_CLEAN_DEP  = $$($(1)_CLEAN_DEP)
$(2)_FCLEAN_DEP = $$($(1)_FCLEAN_DEP)

$(2)_CLEAN      = $$($(1)_CLEAN)
$(2)_FCLEAN     = $$($(1)_FCLEAN)
endef

# auto build directories
# root src files must start with "./"
define __add/build_dir =
.PRECIOUS: $$($(1)_BDIR)/. $$($(1)_BDIR)%/.
$$($(1)_BDIR)/.:
	mkdir -p $$@
$$($(1)_BDIR)%/.:
	mkdir -p $$@
endef

define __add/objrule =
$$($(1)_BDIR)%.o:	%.cpp $$($(1)_C_DEP) | $$$${@D}/.
	$$($(1)_CXX) $$($(1)_CXXFLAGS) $$($(1)_FINC)  -c $$< -o $$@ -MD
endef

#! @1 - project name
#! @2 - out name
define __add/exe =
$$($(1)_BDIR)/$(2): $$($(1)_OBJS) $$($(1)_DEPEND)$$($(1)_LD_DEP)
	$$($(1)_LD) $$($(1)_LDFLAGS) $$($(1)_OBJS) -o $$($(1)_BDIR)/$(2) $$($(1)_LIBS)

.PHONY: $(1)/exe
$(1)/exe: $$($(1)_BDIR)/$(2)

$(1)_FCLEAN  += $$($(1)_BDIR)/$(2)
endef

#! @1 - project name
#! @2 - out name
define __add/lib =
$$($(1)_BDIR)/$(2): $$($(1)_OBJS) $$($(1)_DEPEND) $$($(1)_LD_DEP)
	$(AR) $(2) $$($(1)_OBJS)

.PHONY: $(1)/lib
$(1)/lib: $$($(1)_BDIR)/$(2)

$(1)_FCLEAN  += $$($(1)_BDIR)/$(2)
endef

#! @1 - project name
#! @2 - out name
define __add/shlib =
$$($(1)_BDIR)/$(2): $$($(1)_OBJS) $$($(1)_DEPEND)) $$($(1)_LD_DEP)
	$$($(1)_LD) $$($(1)_LDFLAGS) -shared -Wl,-soname,$(2) -o $$($(1)_BDIR)/$(2) $$($(1)_OBJS) $$($(1)_LIBS)

.PHONY: $(1)/shlib
$(1)/shlib: $$($(1)_BDIR)/$(2)

$(1)_FCLEAN       += $$($(1)_BDIR)/$(2)
endef

define __add/baserules =
.PHONY: $(1)/clean
$(1)/clean: $$($(1)_CLEAN_DEP)
	-${RM} $$($(1)_CLEAN) $$($(1)_OBJS) $$($(1)_DFILES)

.PHONY: $(1)/fclean
$(1)/fclean: $$($(1)_FCLEAN_DEP)
	-${RM} $$($(1)_FCLEAN) $$($(1)_OBJS) $$($(1)_DFILES)
endef

#! @1 - project name
add/project = ${eval ${call __add/project,$(1),$(2)}}

#! @1 - project name
#! @2 - sub project name
add/subproj = ${eval ${call __add/subproj,$(1),$(2)}}

#! @1 - project name
#! @2 - out name
add/exe = ${eval ${call __add/exe,$(1),$(2)}}
add/lib = ${eval ${call __add/lib,$(1),$(2)}}
add/shlib = ${eval ${call __add/shlib,$(1),$(2)}}