# "test/parser/datatype/Makefile.am"
#	$Id: Makefile.am,v 1.7 2005/05/04 23:39:07 fang Exp $

AUTOMAKE_OPTIONS = 1.9

SUFFIXES = .in .artctest .artobjtest .artobjunrolltest

ARTC_TEST_FAILURES = \
	    003 004 005 009 \
	010 011 012 014 015 017 018 019 \
	020 024 025

ARTC_TEST_PASSES = \
	001 002 006 007 008 \
	013 016 \
	021 022 023 026 027 028

ARTC_TEST_SUBJECTS = $(ARTC_TEST_FAILURES) $(ARTC_TEST_PASSES)

ARTOBJ_TEST_SUBJECTS = \
	$(ARTC_TEST_PASSES)

ARTOBJUNROLL_TEST_SUBJECTS = \
	$(ARTOBJ_TEST_SUBJECTS)

TESTS = $(ARTC_TEST_SUBJECTS:=.artctest) \
	$(ARTOBJ_TEST_SUBJECTS:=.artobjtest) \
	$(ARTOBJUNROLL_TEST_SUBJECTS:=.artobjunrolltest)

# more output expectation files for when bison is used
BISON_OUTPUTS = 015 017 018 019 020

# define empty, let template Makefile add to it
EXTRA_DIST =
CLEANFILES =
DISTCLEANFILES =

include $(top_srcdir)/test/Make.test

# dist-hook: dist-hook-extra

