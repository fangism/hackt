# "test/parser/typedef/Makefile.am"

AUTOMAKE_OPTIONS = 1.9

SUFFIXES = .artctest .in

ARTC_TEST_SUBJECTS = \
	    001 002 003 004 005 006 007 008 009 \
	010 011 012 013

TESTS = $(ARTC_TEST_SUBJECTS:=.artctest)

# more output expectation files for when bison is used
BISON_OUTPUTS = 001

# define empty, let template Makefile add to it
EXTRA_DIST =
CLEANFILES =

include $(top_srcdir)/test/Make.test

# dist-hook: dist-hook-extra

