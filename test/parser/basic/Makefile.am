# "test/parser/basic/Makefile.am"
#	$Id: Makefile.am,v 1.9 2005/08/08 16:51:26 fang Exp $

include $(top_srcdir)/test/Make.test-head

ARTC_TEST_FAILURES = \
	004 005 008 009 \
	010 011 012 013

ARTC_TEST_PASSES = \
	000 001 002 003 006 007 014

ARTC_TEST_SUBJECTS = $(ARTC_TEST_FAILURES) $(ARTC_TEST_PASSES)

# should be the subset of ART_TEST_SUBJECTS that type-checks
ARTOBJ_TEST_SUBJECTS = \
	$(ARTC_TEST_PASSES)

# should be the subset of ARTOBJ_TEST_SUBJECTS that passes unroll checks
ARTOBJUNROLL_TEST_SUBJECTS = \
	$(ARTOBJ_TEST_SUBJECTS)

HACKT_CREATE_TEST_SUBJECTS = \
	$(ARTOBJUNROLL_TEST_SUBJECTS)

# more output expectation files for when bison is used
BISON_OUTPUTS += 004 005 008 010

include $(top_srcdir)/test/Make.test-tail

