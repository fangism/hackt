# "Makefile"
# note: this makefile is specifically for BSDmake
# the GNUmake makefile is "GNUmakefile", 
# which should be automatically detected when GNUmake is invoked

# standard project-independent definitions
include Make.stddef

# project-specific definitions
include Make.vars

# define bsdmake-specific recursive make invocation
# shouldn't need MAKEFLAGS?
RECURSIVE_MAKE = $(MAKE) CC="$(CC)" CFLAGS="$(CFLAGS)" LD="$(LD)" \
	LDFLAGS="$(LDFLAGS)" CPPFLAGS="$(CPPFLAGS)"

default: all

# BSDmake-specific
.BEGIN:	makeinfo

artc: makeinfo

# make-independent target dependencies, where most useful information is
include Make.deps

# bsdmake implicitly includes ".depend"
# but should not explicitly include it because it is dynamically generated

