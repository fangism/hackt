# "test/parser/channel/Makefile.am"
#	$Id: Makefile.am,v 1.12.2.2.4.1 2007/01/10 20:14:44 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = 000 003 004 006 007 \
	013 \
	022 023 \
	local-01 local-02 local-03 local-04

HACKT_PARSE_TEST_PASSES = \
	001 002 005 008 009 010 \
	011 012 014 015 016 017 019 \
	020 021 024 \
	connect-001 connect-002 connect-003 connect-004 connect-005 \
	connect-006 connect-007 connect-008 connect-009 connect-010 \
	connect-011 connect-012 \
	connect-013 connect-014 connect-015 connect-016 \
	connect-017 connect-018 connect-019 connect-020 \
	connect-021 connect-022 connect-023 connect-024 \
	connect-025 connect-026 connect-027 connect-028 \
	connect-029 connect-030 connect-031 connect-032 \
	connect-033 connect-034 connect-035 connect-036 \
	connect-037 connect-038 connect-039 connect-040 \
	connect-041 connect-042 connect-043 connect-044 \
	connect-045 connect-046 connect-047 connect-048 \
	connect-049 connect-050 connect-051 connect-052

# should be the subset of HACKT_OBJ_TEST_SUBJECTS that passes unroll checks
# UNROLL tests are obsolete
HACKT_UNROLL_TEST_PASSES = \
	001 002 005 008 009 010 \
	011 012 014 015 016 017 019 \
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
	connect-037 connect-038 connect-039 connect-040 \
	connect-041 connect-042 connect-045 connect-048 \
	connect-051 connect-052

HACKT_CREATE_TEST_FAILURES = \
	connect-033 connect-034 \
	connect-035 connect-036 \
	connect-043 connect-044 \
	connect-046 connect-047 connect-049 connect-050

if USE_UNROLL_PHASE
else
HACKT_CREATE_TEST_FAILURES += $(HACKT_UNROLL_TEST_FAILURES)
endif

HACKT_ALLOC_TEST_PASSES =
#	connect-009 connect-010

# temporary for now:
HACKT_CHPSIM_ALLOC_TEST_SUBJECTS = $(HACKT_CREATE_TEST_PASSES)

# more output expectation files for when bison is used
BISON_OUTPUTS += 000 003 004 006 007 \
	013

# regressions:
# 018 -- dropped support for user-defined channel types until much later

srcdir_EXTRA_DIST += \
	nonmeta-bool-merge.hac nonmeta-bool-split.hac \
	018.hac

include $(top_srcdir)/test/Make.test-tail

