#OPTIONS= -g -Wall -ansi -D__DEBUG__
OPTIONS= -O3 -Wall -ggdb -fopenmp
EXCLUDE+= ./doc
SRCDIRS=src src/3D

#
# This application uses the FOX Toolkit (www.fox-toolkit.org)
# Adapt paths below to your installation of libFOX (development version)
#

SKERNEL ?= $(shell uname -s)
ifneq (,$(findstring MSYS,$(SKERNEL)))
# MSYS (Windows)
    LIBPATHS=../fox-1.7.56/lib/.libs
    INCLUDES=../fox-1.7.56/include
    LIBRARIES=-lFOX-1.7 -lgdi32 -lwsock32 -limm32
else
# LINUX
    LIBPATHS=/usr/lib
    INCLUDES=/usr/include/fox-1.7
    LIBRARIES=-lFOX-1.7
endif

SED_E=sed
ECHO_CALL?=echo
GEN_CALL?=g++

#
# There should be no need to modify the section below
#

include magic.makefile

.PHONY: test

final: bin/fxscene

binProbe:
	mkdir -p bin

bin/fxscene: binProbe $(filter-out %probe.o,$(objects))
	@$(call generate-bin,$@,$(filter-out binProbe,$^) -L$(LIBPATHS) $(LIBRARIES))
	@$(ECHO_CALL) ---;$(ECHO_CALL) Done

deepclean: clean
	-@rm -f bin/fxscene

