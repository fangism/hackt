# "test/parser/prs/Makefile.am"
#	$Id: Makefile.am,v 1.15 2006/12/01 23:30:09 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = \
	022 028 029

HACKT_PARSE_TEST_PASSES = \
	000 001 002 003 004 005 006 007 008 009 \
	010 011 012 013 014 015 016 017 018 019 \
	020 021 023 024 025 026 027 \
	030 031 032 \
	loop-alias-01 loop-alias-02 loop-alias-03 loop-alias-04 loop-alias-05 \
	loop-alias-06 loop-alias-07 \
	loop-ports-01 loop-ports-02 loop-ports-03 loop-ports-04 loop-ports-05 \
	loop-ports-06 loop-ports-07 loop-ports-08 loop-ports-09 loop-ports-10 \
	loop-ports-11 \
	loop-instantiation-01

# TODO: more meta-programming library test cases

HACKT_UNROLL_TEST_PASSES = \
	000 001 002 003 007 008 009 \
	010 011 012 013 014 015 016 017 018 019 \
	020 021 023 024 025 026 \
	030 031 032 \
	loop-alias-01 loop-alias-02 loop-alias-03 loop-alias-04 loop-alias-05 \
	loop-alias-06 loop-alias-07 \
	loop-ports-01 loop-ports-02 loop-ports-03 loop-ports-04 loop-ports-05 \
	loop-ports-06 loop-ports-07 loop-ports-10

HACKT_UNROLL_TEST_FAILURES = \
	004 005 006 \
	027 \
	loop-instantiation-01 \
	loop-ports-08 loop-ports-09 loop-ports-11

HACKT_CREATE_TEST_PASSES = \
	000 001 002 003 007 008 009 \
	010 011 012 013 014 015 016 017 018 019 \
	020 021 023 024 025 026 \
	030 031 032 \
	loop-alias-01 loop-alias-02 loop-alias-03 loop-alias-04 loop-alias-05 \
	loop-alias-06 loop-alias-07 \
	loop-ports-01 loop-ports-02 loop-ports-03 loop-ports-04 loop-ports-05 \
	loop-ports-06 loop-ports-07 loop-ports-10

if USE_UNROLL_PHASE
HACKT_CREATE_TEST_FAILURES =
else
HACKT_CREATE_TEST_FAILURES = $(HACKT_UNROLL_TEST_FAILURES)
endif

HACKT_ALLOC_TEST_PASSES = $(HACKT_CREATE_TEST_PASSES)

HACKT_CFLAT_TEST_SUBJECTS = $(HACKT_ALLOC_TEST_PASSES)

EMPTY_CFLAT_PRS_TESTS += \
	000 001 007 008 009 \
	010 011 012 \
	020 021 023 024 025 \
	030

# more output expectation files for when bison is used
BISON_OUTPUTS +=

# TODO:
srcdir_EXTRA_DIST +=

include $(top_srcdir)/test/Make.test-tail

