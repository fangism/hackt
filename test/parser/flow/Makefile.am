# "test/parser/prs/Makefile.am"
#	$Id: Makefile.am,v 1.1.2.5 2005/10/30 21:25:09 fang Exp $

include $(top_srcdir)/test/Make.test-head

ARTC_TEST_FAILURES = \
	022 028 029

ARTC_TEST_PASSES = \
	000 001 002 003 004 005 006 007 008 009 \
	010 011 012 013 014 015 016 017 018 019 \
	020 021 023 024 025 026 027 \
	030 031

# TODO: more meta-programming library test cases

ARTC_TEST_SUBJECTS = $(ARTC_TEST_FAILURES) $(ARTC_TEST_PASSES)

ARTOBJ_TEST_SUBJECTS = $(ARTC_TEST_PASSES)

ARTOBJUNROLL_TEST_SUBJECTS = $(ARTOBJ_TEST_SUBJECTS)

HACKT_CREATE_TEST_SUBJECTS = \
	000 001 002 003 007 008 009 \
	010 011 012 013 014 015 016 017 018 019 \
	020 021 023 024 025 026 \
	030

HACKT_ALLOC_TEST_SUBJECTS = $(HACKT_CREATE_TEST_SUBJECTS)

HACKT_CFLAT_TEST_SUBJECTS = $(HACKT_ALLOC_TEST_SUBJECTS)

# KNOWN BUG: missing private aliases in cflat (20051030)
# affects 031.in, pending fix on test cases 122-126 in parser/connect...
#	only cflat produces wrong output, earlier phases are fine.

EMPTY_CFLAT_PRS_TESTS = \
	000 001 007 008 009 \
	010 011 012 \
	020 021 023 024 025 \
	030
EMPTY_CFLAT_PRS = $(EMPTY_CFLAT_PRS_TESTS:=.expect-prs)

$(EMPTY_CFLAT_PRS):
	touch $@

all-local: $(EMPTY_CFLAT_PRS)

AUTO_IGNORE += $(EMPTY_CFLAT_PRS)
CLEANFILES += $(EMPTY_CFLAT_PRS)

# more output expectation files for when bison is used
BISON_OUTPUTS +=

include $(top_srcdir)/test/Make.test-tail

