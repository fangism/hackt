# "test/parser/prs/Makefile.am"
#	$Id: Makefile.am,v 1.1.2.2 2005/10/27 22:53:00 fang Exp $

include $(top_srcdir)/test/Make.test-head

ARTC_TEST_FAILURES =

ARTC_TEST_PASSES = 000 001 002 003 004 005 006 007

ARTC_TEST_SUBJECTS = $(ARTC_TEST_FAILURES) $(ARTC_TEST_PASSES)

ARTOBJ_TEST_SUBJECTS = $(ARTC_TEST_PASSES)

ARTOBJUNROLL_TEST_SUBJECTS = $(ARTOBJ_TEST_SUBJECTS)

HACKT_CREATE_TEST_SUBJECTS = 000 001 002 003 007

# known defect reports: unresolved language issues
# dealing with empty loops and negative ranges...
# null-01: waiting for null-expression resolution
# loop-01: (same)

HACKT_ALLOC_TEST_SUBJECTS = $(HACKT_CREATE_TEST_SUBJECTS)

HACKT_CFLAT_TEST_SUBJECTS = $(HACKT_ALLOC_TEST_SUBJECTS)

EMPTY_CFLAT_PRS_TESTS = 000 001 007
EMPTY_CFLAT_PRS = $(EMPTY_CFLAT_PRS_TESTS:=.expect-prs)

$(EMPTY_CFLAT_PRS):
	touch $@

all-local: $(EMPTY_CFLAT_PRS)

AUTO_IGNORE += $(EMPTY_CFLAT_PRS)
CLEANFILES += $(EMPTY_CFLAT_PRS)

# more output expectation files for when bison is used
BISON_OUTPUTS +=

include $(top_srcdir)/test/Make.test-tail

