# "test/parser/template/Makefile.am"
#	$Id: Makefile.am,v 1.5.2.1 2005/02/03 03:35:07 fang Exp $

AUTOMAKE_OPTIONS = 1.9

SUFFIXES = .in .artctest .artobjtest .artobjunrolltest

ARTC_TEST_FAILURES = \
	    003 005 \
	010 013 014 019 \
	020 023 024 025

ARTC_TEST_PASSES = \
	001 002 004 006 007 008 009 \
	011 012 015 016 017 018 \
	021 022

ARTC_TEST_SUBJECTS = $(ARTC_TEST_FAILURES) $(ARTC_TEST_PASSES)

ARTOBJ_TEST_SUBJECTS = \
	$(ARTC_TEST_PASSES)

ARTOBJUNROLL_TEST_SUBJECTS = \
	$(ARTOBJ_TEST_SUBJECTS)

TESTS = $(ARTC_TEST_SUBJECTS:=.artctest) \
	$(ARTOBJ_TEST_SUBJECTS:=.artobjtest) \
	$(ARTOBJUNROLL_TEST_SUBJECTS:=.artobjunrolltest)

# define empty, let template Makefile add to it
EXTRA_DIST =
CLEANFILES =
DISTCLEANFILES =

include $(top_srcdir)/test/Make.test

# dist-hook: dist-hook-extra

