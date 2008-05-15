# "test/parser/basic/Makefile.am"
#	$Id: Makefile.am,v 1.29 2008/05/15 17:01:03 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = \
	004 005 008 009 \
	010 011 012 013 017 018 \
	020 021 023 024 025 \
	oper-03 oper-05 \
	act-002

HACKT_PARSE_TEST_PASSES = \
	000 001 002 003 006 007 \
	014 015 016 019 \
	022 \
	oper-02 oper-04 \
	inst-attrib-01 inst-attrib-02 inst-attrib-03 inst-attrib-04 \
	act-001

HACKT_FLATTEN_TEST_PASSES = \
	015 016 017 018 019 \
	020 021 022 023

HACKT_FLATTEN_PARSE_TEST_PASSES = \
	015 016 019 \
	022

HACKT_FLATTEN_PARSE_TEST_FAILURES = \
	017 018 \
	020 023
# 021: parse failure vs. lex failures difference

HACKT_PARSE_TEST_WARNINGS = \
	oper-01
# NOTE: oper-01 passes with a warning for now

# should be the subset of HACKT_OBJ_TEST_SUBJECTS that passes unroll checks

HACKT_CREATE_TEST_PASSES = \
	$(HACKT_OBJ_TEST_SUBJECTS)

HACKT_CREATE_TEST_FAILURES =

# more output expectation files for when bison is used
BISON_OUTPUTS += 004 005 008 010 017 018 \
	020 021 \
	oper-05

srcdir_EXTRA_DIST += TODO

include $(top_srcdir)/test/Make.test-tail

