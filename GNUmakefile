# "GNUmakefile"
# note: this makefile is specifically for GNUmake
# the BSDmake makefile is "Makefile
# which should be automatically detected when non-GNUmake is invoked

# standard project-independent definitions
include Make.stddef

# project-specific definitions
include Make.vars

# define bsdmake-specific recursive make invocation
# shouldn't need MAKEFLAGS?
RECURSIVE_MAKE = $(MAKE)
# problems with passing parallel build flags -j

# by default builds first target
default: all

all: makeinfo

# make-independent target dependencies, where most useful information is
include Make.deps

# GNUmake needs to explicity include automatically generated dependencies
-include .depend
