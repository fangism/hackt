# "test/parser/basic/Makefile.am"
#	$Id: Makefile.am,v 1.15 2006/07/16 07:25:51 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = \
	004 005 008 009 \
	010 011 012 013 017 018 \
	020 021 023

HACKT_PARSE_TEST_PASSES = \
	000 001 002 003 006 007 \
	014 015 016 019 \
	022 \
	oper-01 oper-02

# NOTE: oper-01 passes with a warning for now

HACKT_PARSE_TEST_SUBJECTS = \
	$(HACKT_PARSE_TEST_FAILURES) \
	$(HACKT_PARSE_TEST_PASSES)

# should be the subset of HACKT_PARSE_TEST_SUBJECTS that type-checks
HACKT_OBJ_TEST_SUBJECTS = \
	$(HACKT_PARSE_TEST_PASSES)

# should be the subset of HACKT_OBJ_TEST_SUBJECTS that passes unroll checks
HACKT_UNROLL_TEST_SUBJECTS = \
	$(HACKT_OBJ_TEST_SUBJECTS)

HACKT_CREATE_TEST_SUBJECTS = \
	$(HACKT_UNROLL_TEST_SUBJECTS)

# more output expectation files for when bison is used
BISON_OUTPUTS += 004 005 008 010 017 018 \
	020 021

# has warning
EXTRA_DIST += oper-01.stderr
EXTRA_DIST += TODO

include $(top_srcdir)/test/Make.test-tail

