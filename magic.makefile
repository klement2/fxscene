# Copyright (C) 2004-2006 fc <gdev@emailaccount.com> - Licenced under the GPL.

# magic.makefile v1.2, Fri Apr 28 19:44:09 CEST 2006
# REQUIREMENT: make >= 3.80 (for recursive calls)

### PATCH 20060519, PRGLANG bug fixed.
### PATCH 20060428, bug fixes.
### PATCH 20060427, dll support added (now support static and dynamic libs).
### PATCH 20050830, optimization of $srcdirs computation time.
### PATCH 20050727, now work with folder symlinks.

# NOTE: This tool were originaly provided with the dLib.

# This makefile is designed to produce all the intermediate object files in
# a project which files are dispatched in several (sub)folders without any
# particular order. To perform this task, this makefile begins by locating
# all the source files, then the dependency rule is computed for every
# generable source (eg: .cc) and stored in a .d/.dld file. The last step is
# to generate the object files (.o/.dlo) according to the depency rules. This
# makefile doesn't create any library or executable, it's just designed to do
# the 'common' part of a library or executable construction, hence the fact
# you must include it in a project-specific makefile which contain the upper
# level rules to build an executable or a library. Use the variables created
# here to get back the location of the object files.
# Static libraries are processed like standard binaries except objects are
# stored in an archive. Dynamic libraries are processed independently and
# require to name files with another extension (e.g. .lib), then just all the
# PIC objects are accessible through $(dl-objects), use $(dl-binaries) in your
# 'final' rule to complete the .so creation.
# The project-specific makefile must also define the 'final' rule.
# Try 'make debug' to see the list of computed data.

# Sample project-specific makefile:
# ---------------------------------
#
#  OPTIONS= -O3 -pedantic -ansi -Wall
#  EXCLUDE= ./doc
#
#  include magic.makefile
#
#  .PHONY: about
#
#  final: app $(dl-binaries)
#
#  app: $(objects)
#  	@$(call generate-bin,$@,$^ -lm -ldl)
#
#  deepclean: clean
#  	-@rm app
#
#  about:
#  	@echo "Sample makefile"

# It is only designed for the gcc compiler, however, by
# changing the used extensions and the compiler name/options,
# it # should work for other compilers (the compiler must be
# able # to compute the dependency rules however!).

# <!> Pathnames with spaces are not supported!

# [customization]
#  -GENSRCEXTS   : extension list of generable source file (eg: .c)
#  -GENDLSEXTS   : extension list of dynamic libraries, they need a separate
#                  process, so don't use .c, instead use .lib for instance.
#  -SRCEXTS      : extension list of source file (eg: .c .cc .h)
#  -INCLUDE      : compiler additional list of common include paths
#  -EXCLUDE      : list of paths to exclude from source file research
#  -OPTIONS      : compiler additional list of common options
#  -PRGLANG      : input source language (default: c)

# [tools usable in 'upper'-makefiles]
# data/
#  -srcdirs      : folder list containing source files (SRCEXTS)
#  -units        : list of generable sources, except dlibs
#  -dl-units     : list of generable dlibs
#  -objects      : list of objects except dlibs
#  -dl-objetcs   : file list of dlib objects
# functions/
#  -generate-bin : generate a binary
#                  @1=target_name @2=objects and additional directives (-L,-l...)
#  -generate-arc : generate a static library
#                  @1=target_name @2=objects
#  -generate-dlb : generate a shared library
#                  @1=target_name @2=objects
#  -objects-in   : return the list of objects in the given folder name
#                  @1=folder name (beginning with ./)

SHELL=/bin/sh

GENSRCEXTS+= .cc .cpp .c .cxx
GENDLSEXTS+= .lib .dll
SRCEXTS+= $(GENSRCEXTS) .h .hh .hpp
INCLUDE+= $(addprefix -I,$(srcdirs))
INCLUDE+= $(addprefix -I,$(INCLUDES))
PRGLANG?= c++
#OPTIONS?= -g -Wall

generate-dep= $(ECHO_CALL) DEP $(1) && $(GEN_CALL) -x $(PRGLANG) -MM $(2) $(INCLUDE) $(OPTIONS) > $(1).dd && $(SED_E) "s,.*:,$(1).o $(1).d:," $(1).dd > $(1).d && rm -f $(1).dd
generate-dl-dep= $(ECHO_CALL) DL-DEP $(1) && $(GEN_CALL) -x $(PRGLANG) -MM $(2) $(INCLUDE) $(OPTIONS) > $(1).dd && $(SED_E) "s,.*:,$(1).dlo $(1).dld:," $(1).dd > $(1).dld && rm -f $(1).dd
generate-obj= $(ECHO_CALL) OBJ $(1) && $(GEN_CALL) -x $(PRGLANG) -c -o $(1) $(2) $(INCLUDE) $(OPTIONS)
generate-dl-obj= $(ECHO_CALL) DL-OBJ $(1) && $(GEN_CALL) -x $(PRGLANG) -fPIC -c -o $(1) $(2) $(INCLUDE) $(OPTIONS)
generate-bin= $(ECHO_CALL) BIN $(1) && $(GEN_CALL) -Wl,--export-dynamic -o $(1) $(2) -L. $(OPTIONS)
generate-arc= $(ECHO_CALL) ARC $(1) && ar -urc $(1) $(2) && ranlib $(1)
generate-dl-bin= $(ECHO_CALL) DL-BIN $(1) && $(GEN_CALL) -shared -o $(1) $(2) $(OPTIONS)

objects-in=$(filter $(1)/%,$(objects))

.PHONY: all final clean debug force

srcdirs:=$(filter-out $(addsuffix %,$(EXCLUDE)) .,$(SRCDIRS))
units=$(foreach e,$(GENSRCEXTS),$(foreach c,$(srcdirs),$(patsubst %$(e),%,$(wildcard $(c)/*$(e)))))
dl-units=$(foreach e,$(GENDLSEXTS),$(foreach c,$(srcdirs),$(patsubst %$(e),%,$(wildcard $(c)/*$(e)))))
deps=$(addsuffix .d,$(units))
dl-deps=$(addsuffix .dld,$(dl-units))
objects=$(addsuffix .o,$(units))
dl-objects=$(addsuffix .dlo,$(dl-units))
dl-binaries=$(foreach e,$(dl-units),$(dir $(e))lib$(basename $(notdir $(e))).so)

all: $(objects) $(dl-objects) final

debug:
	@echo [srcdirs] $(srcdirs)
	@echo [units] $(units)
	@echo [dl-units] $(dl-units)
	@echo [deps] $(deps)
	@echo [dl-deps] $(dl-deps)
	@echo [objects] $(objects)
	@echo [dl-objects] $(dl-objects)
	@echo [dl-binaries] $(dl-binaries)

force: clean all

final:

clean:
	-@rm -f $(deps) $(dl-deps) $(objects) $(dl-objects) $(dl-binaries)
	@$(ECHO_CALL) ---
	@$(ECHO_CALL) Done

src=$(firstword $(wildcard $(foreach e,$(GENSRCEXTS),$(addsuffix $(e),$*))))
dl-src=$(firstword $(wildcard $(foreach e,$(GENDLSEXTS),$(addsuffix $(e),$*))))
dlo-src=$(addsuffix .dlo,$(dir $*)$(subst lib,,$(basename $(notdir $*))))

$(dl-binaries): $(dl-objects)

%.so: $(dlo-src)
	@$(call generate-dl-bin,$@,$(dlo-src))

%.dlo: $(dl-src)
	@$(call generate-dl-obj,$@,$(dl-src))

%.o: $(src)
	@$(call generate-obj,$@,$(src))

%.dld: $(dl-src)
	@$(call generate-dl-dep,$*,$(dl-src))

%.d: $(src)
	@$(call generate-dep,$*,$(src))

-include $(dl-deps)
-include $(deps)
