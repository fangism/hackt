# "test/parser/template/Makefile.am"

AUTOMAKE_OPTIONS = 1.9

SUFFIXES = .in .artctest .artobjtest

ARTC_TEST_SUBJECTS = \
	    001 002 003 004 005 006 007 008 009 \
	010 011 012 013 014 015 016 017 018 019 \
	020 021

ARTOBJ_TEST_SUBJECTS = \
	001 002 004 006 007 008 \
	011 012 015 016 017 018 \
	021

TESTS = $(ARTC_TEST_SUBJECTS:=.artctest) \
	$(ARTOBJ_TEST_SUBJECTS:=.artobjtest)

# define empty, let template Makefile add to it
EXTRA_DIST =
CLEANFILES =

include $(top_srcdir)/test/Make.test

# dist-hook: dist-hook-extra

