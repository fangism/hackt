# "test/parser/array/Makefile.am"
#	$Id: Makefile.am,v 1.4.4.2 2005/01/21 06:10:01 fang Exp $

AUTOMAKE_OPTIONS = 1.9

SUFFIXES = .in .artctest .artobjtest .artobjunrolltest

ARTC_TEST_FAILURES = \
	    003 004 005 006 008 009 \
	011 012 014 017 018 019 \
	020 021 022 023 024 025 026 027 028 \
	030 031 032 033 034 035 036

ARTC_TEST_PASSES = \
	001 002 007 \
	010 013 015 016 \
	029 \
	037

# unfortunately is out of order... to bad $(sort ...) is non-posix
ARTC_TEST_SUBJECTS = $(ARTC_TEST_PASSES) $(ARTC_TEST_FAILURES)

ARTOBJ_TEST_SUBJECTS = \
	$(ARTC_TEST_PASSES)

ARTOBJUNROLL_TEST_SUBJECTS =

TESTS = $(ARTC_TEST_SUBJECTS:=.artctest) \
	$(ARTOBJ_TEST_SUBJECTS:=.artobjtest) \
	$(ARTOBJUNROLL_TEST_SUBJECTS:=.artobjunrolltest)

# define empty, let template Makefile add to it
EXTRA_DIST =
CLEANFILES =
DISTCLEANFILES =

include $(top_srcdir)/test/Make.test

# dist-hook: dist-hook-extra

