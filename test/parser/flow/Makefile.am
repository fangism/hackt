# "test/parser/prs/Makefile.am"
#	$Id: Makefile.am,v 1.6 2006/01/25 02:23:44 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = \
	022 028 029

HACKT_PARSE_TEST_PASSES = \
	000 001 002 003 004 005 006 007 008 009 \
	010 011 012 013 014 015 016 017 018 019 \
	020 021 023 024 025 026 027 \
	030 031 \
	loop-alias-01 loop-alias-02 loop-alias-03 loop-alias-04 loop-alias-05

# TODO: more meta-programming library test cases

HACKT_PARSE_TEST_SUBJECTS = $(HACKT_PARSE_TEST_FAILURES) $(HACKT_PARSE_TEST_PASSES)

HACKT_OBJ_TEST_SUBJECTS = $(HACKT_PARSE_TEST_PASSES)

HACKT_UNROLL_TEST_SUBJECTS = $(HACKT_OBJ_TEST_SUBJECTS)

HACKT_CREATE_TEST_SUBJECTS = \
	000 001 002 003 007 008 009 \
	010 011 012 013 014 015 016 017 018 019 \
	020 021 023 024 025 026 \
	030 031 \
	loop-alias-01 loop-alias-02 loop-alias-03 loop-alias-04 loop-alias-05

HACKT_ALLOC_TEST_SUBJECTS = $(HACKT_CREATE_TEST_SUBJECTS)

HACKT_CFLAT_TEST_SUBJECTS = $(HACKT_ALLOC_TEST_SUBJECTS)

EMPTY_CFLAT_PRS_TESTS += \
	000 001 007 008 009 \
	010 011 012 \
	020 021 023 024 025 \
	030 \
	loop-alias-01 loop-alias-02 loop-alias-03 loop-alias-04 loop-alias-05

# more output expectation files for when bison is used
BISON_OUTPUTS +=

include $(top_srcdir)/test/Make.test-tail

