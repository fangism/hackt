# "test/parser/param/Makefile.am"
#	$Id: Makefile.am,v 1.5.70.1 2005/07/21 01:06:28 fang Exp $

include $(top_srcdir)/test/Make.test-head

ARTC_TEST_FAILURES = \
	    005 006 007 009 \
	010 011 012 013 015 016 018 \
	023 024 025 026 027 029 \
	030 031 032 034 035

# BISON_OUTPUTS +=

# the type-checked subset of the above
ARTC_TEST_PASSES = \
	001 002 003 004 008 \
	014 017 019 \
	020 021 022 028 \
	033

ARTC_TEST_SUBJECTS = $(ARTC_TEST_FAILURES) $(ARTC_TEST_PASSES)

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

include $(top_srcdir)/test/Make.test-tail

