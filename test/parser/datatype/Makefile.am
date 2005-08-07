# "test/parser/datatype/Makefile.am"
#	$Id: Makefile.am,v 1.9.4.1 2005/08/07 01:07:36 fang Exp $

include $(top_srcdir)/test/Make.test-head

ARTC_TEST_FAILURES = \
	    003 004 005 009 \
	010 011 012 014 015 017 018 019 \
	020 024 025

ARTC_TEST_PASSES = \
	001 002 006 007 008 \
	013 016 \
	021 022 023 026 027 028 029 \
	030

ARTC_TEST_SUBJECTS = $(ARTC_TEST_FAILURES) $(ARTC_TEST_PASSES)

ARTOBJ_TEST_SUBJECTS = \
	$(ARTC_TEST_PASSES)

ARTOBJUNROLL_TEST_SUBJECTS = \
	$(ARTOBJ_TEST_SUBJECTS)

# more output expectation files for when bison is used
BISON_OUTPUTS += 015 017 018 019 020

include $(top_srcdir)/test/Make.test-tail

