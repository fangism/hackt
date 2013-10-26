# "test/parser/rte/Makefile.am"

include $(top_srcdir)/test/Make.test-head

# These test cases use 'ebool' instead of @[atomic].
# Once this syntactic layer is implemented, revisit these tests.
EBOOL_TEST_CASES = 000 001 002 003 004 006
srcdir_EXTRA_DIST += $(EBOOL_TEST_CASES:=.hac)

HACKT_PARSE_TEST_FAILURES =

HACKT_PARSE_TEST_WARNINGS =

HACKT_PARSE_TEST_PASSES = \
	000b 001b 002b 003b 004b 005b 006b 007b 008b 009b \
	010b 011b 012b 013b 014b 015b 016b 017b 018b 019b \
	020b

srcdir_EXTRA_DIST +=

HACKT_CREATE_TEST_PASSES = \
	000b 002b 003b 004b 006b \
	012b 014b 015b 017b 018b \
	020b

# expects warnings:
# 020b

HACKT_CREATE_TEST_FAILURES = \
	001b 005b 007b 008b 009b \
	010b 011b 013b 016b 019b

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

HACKT_PRSIMRC_TEST_SCRIPTS_PASSES =

HACKT_PRSIMRC_TEST_SCRIPTS_FAILURES =

HACKNET_TEST_SCRIPTS_PASSES =

# more output expectation files for when bison is used
BISON_OUTPUTS +=

# includes other sources files used by test-cases (but aren't cases themselves)
srcdir_EXTRA_DIST +=

include $(top_srcdir)/test/Make.test-tail

