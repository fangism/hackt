# "test/parser/typedef/Makefile.am"
#	$Id: Makefile.am,v 1.7.4.1 2005/08/07 01:07:38 fang Exp $

include $(top_srcdir)/test/Make.test-head

ARTC_TEST_FAILURES = \
	    001 006 007 008 009 \
	010

ARTC_TEST_PASSES = \
	002 003 004 005 \
	011 012 013

ARTC_TEST_SUBJECTS = $(ARTC_TEST_FAILURES) $(ARTC_TEST_PASSES)

ARTOBJ_TEST_SUBJECTS = \
	$(ARTC_TEST_PASSES)

ARTOBJUNROLL_TEST_SUBJECTS = \
	$(ARTOBJ_TEST_SUBJECTS)

# more output expectation files for when bison is used
BISON_OUTPUTS += 001

include $(top_srcdir)/test/Make.test-tail

