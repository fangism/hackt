# "test/lexer/Makefile.am"
#	$Id: Makefile.am,v 1.1 2005/03/02 00:29:02 fang Exp $

AUTOMAKE_OPTIONS = 1.9

SUFFIXES = .in .artctest

ARTC_TEST_FAILURES = \
	badid-000 badid-001

# ARTC_TEST_PASSES =

ARTC_TEST_SUBJECTS = $(ARTC_TEST_FAILURES)
# $(ARTC_TEST_PASSES)

# should be the subset of ART_TEST_SUBJECTS that type-checks
# ARTOBJ_TEST_SUBJECTS = $(ARTC_TEST_PASSES)

# should be the subset of ARTOBJ_TEST_SUBJECTS that passes unroll checks
# ARTOBJUNROLL_TEST_SUBJECTS = $(ARTOBJ_TEST_SUBJECTS)

TESTS = $(ARTC_TEST_SUBJECTS:=.artctest) \
	$(ARTOBJ_TEST_SUBJECTS:=.artobjtest) \
	$(ARTOBJUNROLL_TEST_SUBJECTS:=.artobjunrolltest)

# more output expectation files for when bison is used
BISON_OUTPUTS =

# define empty, let template Makefile add to it
CLEANFILES =
DISTCLEANFILES =
EXTRA_DIST =

include $(top_srcdir)/test/Make.test

# dist-hook: dist-hook-extra

