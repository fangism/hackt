# "test/parser/channel/Makefile.am"
#	$Id: Makefile.am,v 1.10.2.1 2006/11/16 20:28:49 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = 000 003 004 006 007 \
	013 \
	022 023 \
	local-01 local-02 local-03 local-04

HACKT_PARSE_TEST_PASSES = \
	001 002 005 008 009 010 \
	011 012 014 015 016 017 018 019 \
	020 021 024

# should be the subset of HACKT_OBJ_TEST_SUBJECTS that passes unroll checks
HACKT_UNROLL_TEST_PASSES = \
	001 002 005 008 009 010 \
	011 012 014 015 016 017 018 019 \
	020 021

HACKT_UNROLL_TEST_FAILURES = \
	024

# expected to fail unroll: 024

# more output expectation files for when bison is used
BISON_OUTPUTS += 000 003 004 006 007 \
	013

srcdir_EXTRA_DIST += \
	connect-001 connect-002 connect-003 connect-004 connect-005 \
	connect-006 connect-007 connect-008 connect-009 connect-010 \
	connect-011 connect-012

include $(top_srcdir)/test/Make.test-tail

