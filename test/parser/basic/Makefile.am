# "test/parser/basic/Makefile.am"

AUTOMAKE_OPTIONS = 1.9

SUFFIXES = .in .artctest .artobjtest

ARTC_TEST_SUBJECTS = \
	000 001 002 003 004 005 006 007 008 009 \
	010 011 012 013

# should be the subset of ART_TEST_SUBJECTS that type-checks
ARTOBJ_TEST_SUBJECTS = \
	000 001 002 003 006

TESTS = $(ARTC_TEST_SUBJECTS:=.artctest) \
	$(ARTOBJ_TEST_SUBJECTS:=.artobjtest)

# more output expectation files for when bison is used
BISON_OUTPUTS = 004 005 008 010

# define empty, let template Makefile add to it
CLEANFILES = 
EXTRA_DIST = 

include $(top_srcdir)/test/Make.test

# dist-hook: dist-hook-extra

