# "test/parser/channel/Makefile.am"
#	$Id: Makefile.am,v 1.24 2011/03/24 19:44:53 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACO_FLAGS += -f case-collision=ignore

HACKT_PARSE_TEST_FAILURES = 000 003 004 006 007 \
	013 \
	022 023 \
	local-01 local-02 local-03 local-04 \
	dataless-06 \
	shared-16 shared-17

HACKT_PARSE_TEST_PASSES = \
	001 002 005 008 009 010 \
	011 012 014 015 016 017 018 019 \
	020 021 024 025 026 027 027b 028 029 \
	030 031 032 033 034 035 036 037 038 039 \
	040 041 042 043 044 044b 045 045b 046 047 048 049 \
	050 051 052 053 054 055 056 057 \
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
	connect-049 connect-050 connect-051 connect-052 \
	dataless-01 dataless-02 dataless-03 dataless-04 dataless-05 \
	dataless-05b \
	shared-01 shared-02 shared-03 shared-04 \
	shared-05 shared-06 shared-07 shared-08 \
	shared-09 shared-10 shared-11 shared-12 \
	shared-13 shared-14 shared-15 \
	act-002 act-003
# pending: act-001
# set/get should be rejected
srcdir_EXTRA_DIST += act-001.hac

# HACKT_UNROLL_TEST_PASSES = \
# 	001 002 005 008 009 010 \
# 	011 012 014 015 016 017 019 \
# 	020 021 \
# 	connect-001 connect-002 connect-005 connect-006 \
# 	connect-009 connect-010 \
# 	connect-017 connect-018 connect-019 connect-020 \
# 	connect-023 connect-024 connect-027 connect-028 \
# 	connect-029 connect-030 connect-031 connect-032 \
# 	connect-033 connect-034 connect-035 connect-036

# TODO: other unroll-passes
HACKT_CHPSIM_ALLOC_TEST_SUBJECTS = \
	025 \
	connect-001 connect-002 connect-005 connect-006 \
	connect-009 connect-010 \
	connect-017 connect-018 connect-019 connect-020 \
	connect-023 connect-024 connect-027 connect-028 \
	connect-029 connect-030 connect-031 connect-032 \
	connect-037 connect-038 connect-039 connect-040 \
	connect-041 connect-042 connect-045 connect-048 \
	connect-051 connect-052 \
	dataless-01 dataless-02 dataless-03 dataless-05 dataless-05b

HACKT_CREATE_TEST_PASSES = $(HACKT_CHPSIM_ALLOC_TEST_SUBJECTS) \
	027 027b 028 029 \
	030 031 032 033 034 035 036 037 038 039 \
	044 045 046 047 048 049 \
	054 055 056 057 \
	shared-01 shared-02 shared-03 shared-04 \
	shared-05 shared-06 shared-07 shared-08 shared-13
# pending: act-003
# pending: 018 -- faking user-defined channel as a process now


HACKT_CREATE_TEST_FAILURES = \
	024 \
	040 041 042 043 044b 045b \
	050 051 052 053 \
	connect-003 connect-004 connect-007 connect-008 \
	connect-011 connect-012 \
	connect-013 connect-014 connect-015 connect-016 \
	connect-021 connect-022 \
	connect-025 connect-026 \
	connect-033 connect-034 \
	connect-035 connect-036 \
	connect-043 connect-044 \
	connect-046 connect-047 connect-049 connect-050 \
	dataless-04 \
	shared-09 shared-10 shared-11 shared-12 \
	shared-14 shared-15 \
	act-003

HACKT_ALLOC_TEST_PASSES = $(HACKT_CREATE_TEST_PASSES)

HACKT_CFLAT_TEST_SUBJECTS = \
	027


HACKT_CHPSIMRC_TEST_SCRIPTS_PASSES = \
	connect-001-a connect-009-a \
	connect-010-a \
	connect-027-a connect-028-a connect-029-a \
	connect-030-a connect-031-a connect-032-a \
	connect-038-a connect-038-b connect-038-c \
	connect-039-a \
	connect-045-a \
	025-a 025-b 025-c

# more output expectation files for when bison is used
BISON_OUTPUTS += 000 003 004 006 007 \
	013 \
	local-01 local-02 local-03 local-04

srcdir_EXTRA_DIST += \
	nonmeta-bool-merge.hac nonmeta-bool-split.hac \
	source-sink.hac

# TODO: add these once member_nonmeta_instance_references is implemented
srcdir_EXTRA_DIST += \
	connect-053.hac connect-053a.hac connect-053b.hac

include $(top_srcdir)/test/Make.test-tail

