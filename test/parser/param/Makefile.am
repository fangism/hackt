# "test/parser/param/Makefile.am"
#	$Id: Makefile.am,v 1.4.4.2 2005/01/21 02:48:15 fang Exp $

AUTOMAKE_OPTIONS = 1.9

SUFFIXES = .in .artctest .artobjtest .artobjunrolltest

ARTC_TEST_SUBJECTS = \
	    001 002 003 004 005 006 007 008 009 \
	010 011 012 013 014 015 016 017 018 019 \
	020 021 022 023 024 025 026 027 028 029 \
	030 031 032 033 034 035

BISON_OUTPUTS =

# the type-checked subset of the above
ARTC_TEST_PASSES = \
	001 002 003 004 008 \
	014 017 019 \
	020 021 022 028 \
	033

ARTOBJ_TEST_SUBJECTS = \
	$(ARTC_TEST_PASSES)

# subset of ARTOBJ_TEST_SUBJECTS
ARTOBJUNROLL_TEST_SUBJECTS = \
	$(ARTOBJ_TEST_SUBJECTS)

# not needed yet, parse/type-check/unroll failures already handled by scripts
ARTOBJUNROLL_XFAIL_TEST_SUBJECTS =

TESTS = $(ARTC_TEST_SUBJECTS:=.artctest) \
	$(ARTOBJ_TEST_SUBJECTS:=.artobjtest) \
	$(ARTOBJUNROLL_TEST_SUBJECTS:=.artobjunrolltest)

XFAIL_TESTS = $(ARTC_XFAIL_TEST_SUBJECTS:=.artctest) \
	$(ARTOBJ_XFAIL_TEST_SUBJECTS:=.artobjtest) \
	$(ARTOBJUNROLL_XFAIL_TEST_SUBJECTS:=.artobjunrolltest)

# define empty, let template Makefile add to it
EXTRA_DIST =
CLEANFILES =
DISTCLEANFILES =

include $(top_srcdir)/test/Make.test

# dist-hook: dist-hook-extra

