# "test/parser/param/Makefile.am"
#	$Id: Makefile.am,v 1.7 2005/08/08 16:51:48 fang Exp $

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

include $(top_srcdir)/test/Make.test-tail

