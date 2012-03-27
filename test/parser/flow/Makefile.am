# "test/parser/prs/Makefile.am"
#	$Id: Makefile.am,v 1.21 2010/08/24 21:05:51 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = \
	022 028 029 \
	loop-expr-03 loop-expr-06 \
	bad-loop-01 bad-loop-02 bad-loop-03 bad-loop-04 bad-loop-05

HACKT_PARSE_TEST_PASSES = \
	000 001 002 003 004 005 006 007 008 009 \
	010 011 012 013 014 015 016 017 018 019 \
	020 021 023 024 025 026 027 \
	030 031 031b 032 033 \
	loop-alias-01 loop-alias-02 loop-alias-03 loop-alias-04 loop-alias-05 \
	loop-alias-06 loop-alias-07 \
	loop-ports-01 loop-ports-02 loop-ports-03 loop-ports-04 loop-ports-05 \
	loop-ports-06 loop-ports-07 loop-ports-08 loop-ports-09 loop-ports-10 \
	loop-ports-11 \
	loop-instantiation-01 \
	loop-expr-01 loop-expr-02 loop-expr-04 loop-expr-05 \
	loop-expr-07 loop-expr-08 loop-expr-09 \
	loop-spec-01 loop-spec-02 loop-spec-03 loop-spec-04 loop-spec-05 \
	cond-inst-01 cond-inst-02 cond-inst-03 \
	cond-spec-01 cond-spec-02 cond-spec-03 cond-spec-04 cond-spec-05 \
	cond-spec-02b cond-spec-03b cond-spec-04b cond-spec-05b

# TODO: more meta-programming library test cases

HACKT_CREATE_TEST_FAILURES = \
	004 005 006 \
	027 \
	loop-instantiation-01 \
	loop-ports-08 loop-ports-09 loop-ports-11

HACKT_CREATE_TEST_PASSES = \
	000 001 002 003 007 008 009 \
	010 011 012 013 014 015 016 017 018 019 \
	020 021 023 024 025 026 \
	030 031 031b 032 033 \
	loop-alias-01 loop-alias-02 loop-alias-03 loop-alias-04 loop-alias-05 \
	loop-alias-06 loop-alias-07 \
	loop-ports-01 loop-ports-02 loop-ports-03 loop-ports-04 loop-ports-05 \
	loop-ports-06 loop-ports-07 loop-ports-10 \
	loop-expr-01 loop-expr-02 loop-expr-04 loop-expr-05 \
	loop-expr-07 loop-expr-08 loop-expr-09 \
	loop-spec-01 loop-spec-02 loop-spec-03 loop-spec-04 loop-spec-05 \
	cond-inst-01 cond-inst-02 cond-inst-03 \
	cond-spec-01 cond-spec-02 cond-spec-03 cond-spec-04 cond-spec-05 \
	cond-spec-02b cond-spec-03b cond-spec-04b cond-spec-05b

HACKT_ALLOC_TEST_PASSES = $(HACKT_CREATE_TEST_PASSES)

HACKT_CFLAT_TEST_SUBJECTS = $(HACKT_ALLOC_TEST_PASSES)

EMPTY_CFLAT_PRS_TESTS += \
	000 001 007 008 009 \
	010 011 012 \
	020 021 023 024 025 \
	030 \
	loop-expr-01 loop-expr-02 loop-expr-04 loop-expr-05 \
	loop-expr-07 loop-expr-08 loop-expr-09

HACKT_PRSIMRC_TEST_SCRIPTS_PASSES = \
	loop-spec-05

# more output expectation files for when bison is used
BISON_OUTPUTS +=

# TODO:
srcdir_EXTRA_DIST +=

include $(top_srcdir)/test/Make.test-tail

