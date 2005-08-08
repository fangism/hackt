# "test/parser/channel/Makefile.am"
#	$Id: Makefile.am,v 1.7 2005/08/08 16:51:27 fang Exp $

include $(top_srcdir)/test/Make.test-head

ARTC_TEST_FAILURES = 000 003 004 006 007 \
	013 \
	022 023

ARTC_TEST_PASSES = 001 002 005 008 009 010 \
	011 012 014 015 016 017 018 019 \
	020 021
# 024 pending...

ARTC_TEST_SUBJECTS = $(ARTC_TEST_FAILURES) $(ARTC_TEST_PASSES)

# should be the subset of ART_TEST_SUBJECTS that type-checks
ARTOBJ_TEST_SUBJECTS = \
	$(ARTC_TEST_PASSES)

# should be the subset of ARTOBJ_TEST_SUBJECTS that passes unroll checks
ARTOBJUNROLL_TEST_SUBJECTS = \
	$(ARTOBJ_TEST_SUBJECTS)

# more output expectation files for when bison is used
BISON_OUTPUTS += 000 003 004 006 007 \
	013

include $(top_srcdir)/test/Make.test-tail

