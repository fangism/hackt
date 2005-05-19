# "test/parser/prs/Makefile.am"
#	$Id: Makefile.am,v 1.1.2.1 2005/05/19 02:54:32 fang Exp $

AUTOMAKE_OPTIONS = 1.9

SUFFIXES = .in .artctest .artobjtest .artobjunrolltest

ARTC_TEST_FAILURES = inv-01 inv-04 inv-05 inv-10 inv-11

ARTC_TEST_PASSES = inv-02 inv-03 inv-06 inv-07 inv-08 inv-09 \
	inv-12 inv-13 \
	nand-01 nand-02 nand-03 nor-01 xor-01

ARTC_TEST_SUBJECTS = $(ARTC_TEST_FAILURES) $(ARTC_TEST_PASSES)

ARTOBJ_TEST_SUBJECTS = $(ARTC_TEST_PASSES)

ARTOBJUNROLL_TEST_SUBJECTS = $(ARTOBJ_TEST_SUBJECTS)

TESTS = $(ARTC_TEST_SUBJECTS:=.artctest) \
	$(ARTOBJ_TEST_SUBJECTS:=.artobjtest) \
	$(ARTOBJUNROLL_TEST_SUBJECTS:=.artobjunrolltest)

# more output expectation files for when bison is used
BISON_OUTPUTS = inv-01 inv-11

# define empty, let template Makefile add to it
EXTRA_DIST =
CLEANFILES =
DISTCLEANFILES =

include $(top_srcdir)/test/Make.test

# dist-hook: dist-hook-extra

