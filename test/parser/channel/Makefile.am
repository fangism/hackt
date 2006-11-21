# "test/parser/channel/Makefile.am"
#	$Id: Makefile.am,v 1.11 2006/11/21 22:39:21 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = 000 003 004 006 007 \
	013 \
	022 023 \
	local-01 local-02 local-03 local-04

HACKT_PARSE_TEST_PASSES = \
	001 002 005 008 009 010 \
	011 012 014 015 016 017 018 019 \
	020 021 024 \
	connect-001 connect-002 connect-003 connect-004 connect-005 \
	connect-006 connect-007 connect-008 connect-009 connect-010 \
	connect-011 connect-012 \
	connect-013 connect-014 connect-015 connect-016 \
	connect-017 connect-018 connect-019 connect-020 \
	connect-021 connect-022 connect-023 connect-024 \
	connect-025 connect-026 connect-027 connect-028 \
	connect-029 connect-030 connect-031 connect-032 \
	connect-033 connect-034 connect-035 connect-036

# should be the subset of HACKT_OBJ_TEST_SUBJECTS that passes unroll checks
HACKT_UNROLL_TEST_PASSES = \
	001 002 005 008 009 010 \
	011 012 014 015 016 017 018 019 \
	020 021 \
	connect-001 connect-002 connect-005 connect-006 \
	connect-009 connect-010 \
	connect-017 connect-018 connect-019 connect-020 \
	connect-023 connect-024 connect-027 connect-028 \
	connect-029 connect-030 connect-031 connect-032 \
	connect-033 connect-034 connect-035 connect-036

HACKT_UNROLL_TEST_FAILURES = \
	024 \
	connect-003 connect-004 connect-007 connect-008 \
	connect-011 connect-012 \
	connect-013 connect-014 connect-015 connect-016 \
	connect-021 connect-022 \
	connect-025 connect-026

# TODO: other unroll-passes
HACKT_CREATE_TEST_PASSES = \
	connect-001 connect-002 connect-005 connect-006 \
	connect-009 connect-010 \
	connect-017 connect-018 connect-019 connect-020 \
	connect-023 connect-024 connect-027 connect-028 \
	connect-029 connect-030 connect-031 connect-032 \
	connect-033 connect-034

HACKT_CREATE_TEST_FAILURES = \
	connect-035 connect-036

HACKT_ALLOC_TEST_PASSES =
#	connect-009 connect-010

# more output expectation files for when bison is used
BISON_OUTPUTS += 000 003 004 006 007 \
	013

srcdir_EXTRA_DIST += \
	nonmeta-bool-merge.hac nonmeta-bool-split.hac

include $(top_srcdir)/test/Make.test-tail

