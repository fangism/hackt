# "test/parser/rte/Makefile.am"

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = \
	invariant-02 invariant-03

HACKT_PARSE_TEST_WARNINGS =

HACKT_PARSE_TEST_PASSES = \
	000b 001b 002b 003b 004b 005b 006b 007b 008b 009b \
	010b 011b 012b 013b 014b 015b 016b 017b 018b 019b \
	020b 021b 022b 023b 024b 025b 026b 027b 028b 029b \
	030b 031b 032b 033b 034b \
	000 001 002 003 004 005 006 007 008 009 \
	010 011 012 013 014 015 016 017 018 019 \
	020 021 022 023 024 025 026 027 028 029 \
	030 031 032 033 034 035 036 037 038 039 \
	040 \
	invariant-01

srcdir_EXTRA_DIST +=

HACKT_CREATE_TEST_PASSES = \
	000b 002b 003b 004b 006b \
	012b 014b 015b 017b 018b \
	020b 022b 026b 027b 029b \
	000 002 003 004 006 \
	012 014 015 017 018 \
	020 022 026 027 029 \
	035 036 037 038 \
	invariant-01

# expects warnings:
# 020b

HACKT_CREATE_TEST_FAILURES = \
	001b 005b 007b 008b 009b \
	010b 011b 013b 016b 019b \
	021b 023b 024b 025b 028b \
	030b 031b 032b 033b 034b \
	001 005 007 008 009 \
	010 011 013 016 019 \
	021 023 024 025 028 \
	030 031 032 033 034 039 \
	040

# known defect reports: unresolved language issues
# dealing with empty loops and negative ranges...
# null-01: waiting for null-expression resolution
# loop-01: (same)

HACKT_CFLAT_TEST_SUBJECTS =

HACKT_CFLAT_LVS_TEST_PASSES =

HACKT_CFLAT_LVS_TEST_FAILURES =

HACKT_ALLOC_TEST_PASSES = $(HACKT_CFLAT_TEST_SUBJECTS)

HACKT_CFLAT_SPRS_TEST_SUBJECTS =

# subset of HACKT_CFLAT_TEST_SUBJECTS
HACKT_PRSIMEXPR_TEST_SUBJECTS =

# expecting empty .prs
EMPTY_CFLAT_PRS_TESTS +=

# We do not want to trace prsim tests in this directory -- disable
PRSIMRC_AUTOTRACE_DISABLE_HOOK = autotrace=0;

HACKT_PRSIMRC_TEST_SCRIPTS_PASSES = \
	000b \
	invariant-01

HACKT_PRSIMRC_TEST_SCRIPTS_FAILURES =

HACKNET_TEST_SCRIPTS_PASSES =

# more output expectation files for when bison is used
BISON_OUTPUTS +=

# includes other sources files used by test-cases (but aren't cases themselves)
srcdir_EXTRA_DIST +=

include $(top_srcdir)/test/Make.test-tail

