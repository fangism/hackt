# "test/parser/prs/Makefile.am"
#	$Id: Makefile.am,v 1.33.2.4 2007/09/29 19:44:29 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = \
	inv-04 inv-05 inv-10 inv-11 \
	celem-07 celem-08 \
	echo-02 echo-03 echo-04 \
	after-01 \
	spec-01 spec-03 spec-05 \
	macro-pass-04 macro-pass-06 \
	macro-param-01 macro-param-02 macro-param-03 \
	size-02 size-06 \
	spec_min_sep-02 spec_min_sep-07 spec_min_sep-08 spec_min_sep-09 \
	loop-05 loop-07 \
	cond-01 \
	top-level-02 top-level-03 \
	precharge-01

HACKT_PARSE_TEST_PASSES = \
	inv-01 inv-02 inv-03 inv-06 inv-07 inv-08 inv-09 \
	inv-12 inv-13 inv-14 inv-15 inv-16 inv-17 inv-18 \
	nand-01 nand-02 nand-03 nand-04 nand-05 nand-06 nand-07 \
	nor-01 xor-01 \
	celem-01 celem-02 celem-03 celem-04 celem-05 celem-06 \
	wchb-01 wchb-02 wchb-03 wchb-04 wchb-05 \
	null-00 null-01 \
	pal-01 pal-02 pal-03 \
	loop-00 loop-01 loop-02 loop-03 loop-04 loop-06 loop-08 loop-09 \
	cond-02 cond-03 cond-04 cond-05 cond-06 cond-07 \
	short-01 short-02 short-03 short-04 short-05 \
	echo-01 echo-05 \
	macro-pass-01 macro-pass-02 macro-pass-03 macro-pass-05 macro-pass-07 \
	after-02 after-03 \
	spec-02 spec-04 spec-06 spec-07 \
	size-01 size-03 size-04 size-05 \
	weak-01 weak-02 \
	spec_min_sep-01 spec_min_sep-03 spec_min_sep-04 spec_min_sep-05 \
	spec_min_sep-06 spec_min_sep-10 spec_min_sep-11 \
	bug-001 \
	top-level-01\
	lvs-order-01 lvs-order-02 lvs-order-03 \
	lvs-xcpl-inv-01 \
	tree-00 tree-01 tree-02 tree-03 tree-04 \
	subckt-00 subckt-01 \
	subckt-02 subckt-03 \
	subckt-04 subckt-05 \
	subckt-06 \
	internal-node-00 \
	precharge-00 \
	unstab-01 \
	prs-param-01 \
	act-001 act-002 act-003 act-004 act-005 \
	act-006 act-007 act-010
# pending: act-008 act-009

HACKT_CREATE_TEST_PASSES = \
	inv-01 \
	inv-14 inv-15 inv-16 inv-17 inv-18 \
	celem-01 celem-02 celem-03 celem-04 celem-05 celem-06 \
	nand-04 nand-05 nand-06 nand-07 \
	wchb-01 wchb-02 wchb-03 wchb-04 wchb-05 \
	null-00 \
	pal-01 pal-02 pal-03 \
	loop-00 loop-04 loop-06 loop-08 loop-09 \
	cond-02 cond-03 cond-04 cond-05 cond-06 cond-07 \
	short-01 short-02 short-03 short-04 short-05 \
	echo-01 echo-05 \
	macro-pass-01 macro-pass-02 macro-pass-03 macro-pass-05 macro-pass-07 \
	after-02 after-03 \
	spec-02 spec-04 spec-06 spec-07 \
	size-01 size-03 size-04 size-05 \
	weak-01 weak-02 \
	spec_min_sep-01 spec_min_sep-03 spec_min_sep-04 spec_min_sep-05 \
	spec_min_sep-06 spec_min_sep-10 spec_min_sep-11 \
	bug-001 \
	top-level-01 \
	lvs-order-01 lvs-order-02 lvs-order-03 \
	lvs-xcpl-inv-01 \
	tree-02 tree-03 tree-04 \
	subckt-01 subckt-03 subckt-05 subckt-06 \
	unstab-01 \
	act-001 act-002 act-006 act-007 act-010

# TODO: precharge-00 etc. to middle and back-end tests
# pending: act-003 act-004 act-005

HACKT_CREATE_TEST_FAILURES = \
	loop-01 loop-02 loop-03 \
	null-01

# known defect reports: unresolved language issues
# dealing with empty loops and negative ranges...
# null-01: waiting for null-expression resolution
# loop-01: (same)

HACKT_CFLAT_TEST_SUBJECTS = \
	inv-14 inv-15 inv-16 inv-17 inv-18 \
	celem-01 celem-02 celem-03 celem-04 celem-05 celem-06 \
	nand-04 nand-05 nand-06 nand-07 \
	wchb-01 wchb-02 wchb-03 wchb-04 wchb-05 \
	null-00 \
	pal-01 pal-02 pal-03 \
	loop-00 loop-04 loop-06 loop-08 loop-09 \
	cond-02 cond-03 cond-04 cond-05 cond-06 cond-07 \
	short-01 short-02 short-03 short-04 short-05 \
	echo-01 echo-05 \
	macro-pass-01 macro-pass-02 macro-pass-03 macro-pass-05 macro-pass-07 \
	after-02 after-03 \
	spec-02 spec-04 spec-06 spec-07 \
	size-01 size-03 size-04 size-05 \
	weak-01 weak-02 \
	bug-001 \
	top-level-01 \
	tree-02 tree-03 \
	subckt-01 \
	unstab-01 \
	act-001 act-002 act-006 act-007 act-010
# pending: act-003 act-004 act-005

HACKT_CFLAT_LVS_TEST_PASSES = \
	nand-04 nand-08 nand-09 \
	lvs-order-01 lvs-order-03 \
	lvs-xcpl-inv-01

HACKT_CFLAT_LVS_TEST_FAILURES = \
	lvs-order-02

HACKT_CFLAT_SEU_TEST_SUBJECTS = \
	spec_min_sep-01 spec_min_sep-03 spec_min_sep-04 spec_min_sep-05 \
	spec_min_sep-06 spec_min_sep-10 spec_min_sep-11

HACKT_ALLOC_TEST_PASSES = \
	$(HACKT_CFLAT_TEST_SUBJECTS) \
	$(HACKT_CFLAT_SEU_TEST_SUBJECTS)
#	$(HACKT_CFLAT_LVS_TEST_PASSES)

HACKT_CFLAT_SPRS_TEST_SUBJECTS = \
	size-01 size-03 size-04 size-05 \
	act-010
# pending: act-004.hac act-005

# subset of HACKT_CFLAT_TEST_SUBJECTS
HACKT_PRSIMEXPR_TEST_SUBJECTS = inv-14 \
	celem-01 \
	short-01 short-02 short-03 short-04 short-05

# expecting empty .prs
EMPTY_CFLAT_PRS_TESTS += \
	null-00 loop-00 \
	cond-05 \
	macro-pass-01

# more output expectation files for when bison is used
BISON_OUTPUTS += inv-01 inv-11 \
	echo-02 after-01 size-02 \
	spec_min_sep-07 spec_min_sep-08 spec_min_sep-09 \
	cond-01 \
	loop-05 loop-07 \
	top-level-03 \
	precharge-01

# includes other sources files used by test-cases (but aren't cases themselves)
srcdir_EXTRA_DIST += celem.hac nand.hac nor.hac logic.hac

include $(top_srcdir)/test/Make.test-tail

