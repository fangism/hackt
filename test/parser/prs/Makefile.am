# "test/parser/prs/Makefile.am"
#	$Id: Makefile.am,v 1.58.2.1 2010/02/25 21:28:22 fang Exp $

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
	spec_min_sep-02 spec_min_sep-02b spec_min_sep-02c \
	spec_min_sep-07 spec_min_sep-08 spec_min_sep-09 \
	top-level-02 top-level-03 \
	subckt-02 subckt-03 \
	precharge-01 \
	internal-node-01 internal-node-02 internal-node-03 internal-node-04 \
	internal-node-05 internal-node-08 \
	internal-node-11 internal-node-16 \
	internal-node-18 \
	runmodestatic-02 \
	flavor-02 \
	supply-07 supply-08 supply-09 supply-10 supply-11 supply-12

HACKT_PARSE_TEST_WARNINGS = \
	macro-attrib-01 macro-attrib-02

HACKT_PARSE_TEST_PASSES = \
	inv-01 inv-02 inv-03 inv-06 inv-07 inv-08 inv-09 \
	inv-12 inv-13 inv-14 inv-15 inv-16 inv-17 inv-18 \
	nand-01 nand-02 nand-03 nand-04 nand-05 nand-06 nand-07 \
	nor-01 xor-01 \
	celem-01 celem-02 celem-03 celem-04 celem-05 celem-06 \
	wchb-01 wchb-02 wchb-03 wchb-04 wchb-05 \
	null-00 null-01 \
	pal-01 pal-02 pal-03 \
	loop-00 loop-01 loop-02 loop-03 loop-04 loop-05 loop-06 loop-07 \
	loop-08 loop-09 loop-10 loop-11 loop-12 loop-13 loop-14 loop-15 \
	loop-16 loop-17 loop-18 loop-19 loop-20 \
	cond-01 cond-02 cond-03 cond-04 cond-05 cond-06 cond-07 \
	cond-08 cond-09 cond-10 cond-11 cond-12 cond-13 cond-14 \
	cond-15 cond-16 cond-17 cond-18 \
	short-01 short-02 short-03 short-04 short-05 \
	echo-01 echo-05 \
	macro-pass-01 macro-pass-02 macro-pass-03 macro-pass-05 macro-pass-07 \
	macro-pass-05b macro-pass-07b \
	macro-pass-05c macro-pass-07c \
	macro-pass-08 macro-pass-09 \
	after-02 after-03 \
	spec-02 spec-04 spec-06 spec-07 \
	size-01 size-03 size-04 size-05 \
	weak-01 weak-02 weak-03 \
	spec_min_sep-01 spec_min_sep-03 spec_min_sep-04 spec_min_sep-05 \
	spec_min_sep-06 spec_min_sep-10 spec_min_sep-11 \
	bug-001 \
	top-level-01 top-level-04 \
	flat-type-01 flat-type-02 flat-type-03 \
	lvs-order-01 lvs-order-02 lvs-order-03 \
	lvs-xcpl-inv-01 \
	tree-00 tree-01 tree-02 tree-03 tree-04 \
	subckt-00 subckt-01 \
	subckt-04 subckt-05 \
	subckt-06 \
	subckt-07 subckt-08 \
	subckt-09 subckt-10 \
	subckt-11 subckt-12 \
	subckt-13 subckt-14 subckt-15 subckt-16 \
	precharge-00 \
	precharge-02 \
	unstab-01 unstab-02 \
	comb-01 \
	iscomb-01 iscomb-02 iscomb-03 iscomb-04 iscomb-05 iscomb-06 \
	iscomb-07 iscomb-08 iscomb-09 iscomb-10 iscomb-11 iscomb-12 \
	iscomb-13 iscomb-14 iscomb-15 iscomb-16 iscomb-17 iscomb-18 \
	iscomb-19 iscomb-20 iscomb-21 iscomb-22 iscomb-23 iscomb-24 \
	iscomb-25 iscomb-26 iscomb-26-a iscomb-26-b \
	iscomb-27 \
	iscomb-27-a iscomb-27-b iscomb-27-c iscomb-27-d iscomb-27-e \
	iscomb-27-f iscomb-27-g iscomb-27-h iscomb-27-i iscomb-27-j \
	iscomb-27-k iscomb-27-l iscomb-27-m iscomb-27-n iscomb-27-o \
	iscomb-27-p iscomb-27-q \
	keeper-01 \
	iskeeper-01 iskeeper-02 iskeeper-03 iskeeper-04 \
	isckeeper-01 isckeeper-02 \
	autokeeper-01 autokeeper-02 autokeeper-03 autokeeper-04 \
	isrvc-01 \
	interference-01 interference-02 interference-03 \
	prs-param-01 \
	act-001 act-002 act-003 act-004 act-005 \
	act-006 act-007 act-009 act-010 \
	connect-01 connect-02 connect-03 \
	internal-node-00 internal-node-06 internal-node-07 internal-node-09 \
	internal-node-10 internal-node-12 internal-node-13 internal-node-14 \
	internal-node-15 internal-node-17 \
	auto-celem-00 auto-celem-01 auto-celem-02 auto-celem-03 auto-celem-04 \
	auto-celem-05 auto-celem-06 auto-celem-07 auto-celem-08 auto-celem-09 \
	auto-celem-10 auto-celem-11 auto-celem-12 auto-celem-13 \
	strengths-01 strengths-02 \
	invariant-01 invariant-02 invariant-03 invariant-04 \
	flavor-01 flavor-03 flavor-04 flavor-05 \
	supply-01 supply-02 supply-03 supply-04 supply-05 supply-06 \
	runmodestatic-01 runmodestatic-03 runmodestatic-04 runmodestatic-05 \
	reff-01 \
	load_cap-01 
# pending (transistor type param): act-008 rule-type{01,02,03,04,05,06}

srcdir_EXTRA_DIST += \
	act-008.hac \
	rule-type-01.hac rule-type-02.hac rule-type-03.hac rule-type-04.hac \
	rule-type-05.hac rule-type-06.hac \
	internal-node-subckt-01.hac

HACKT_CREATE_TEST_PASSES = \
	inv-01 \
	inv-14 inv-15 inv-16 inv-17 inv-18 \
	celem-01 celem-02 celem-03 celem-04 celem-05 celem-06 \
	nand-04 nand-05 nand-06 nand-07 \
	wchb-01 wchb-02 wchb-03 wchb-04 wchb-05 \
	null-00 \
	pal-01 pal-02 pal-03 \
	loop-00 loop-04 loop-05 loop-06 loop-07 loop-08 loop-09 \
	loop-10 loop-14 loop-15 loop-16 loop-18 loop-19 loop-20 \
	cond-01 cond-02 cond-03 cond-04 cond-05 cond-06 cond-07 \
	cond-08 cond-09 cond-10 cond-11 cond-12 cond-13 cond-14 \
	cond-15 cond-16 cond-17 cond-18 \
	short-01 short-02 short-03 short-04 short-05 \
	echo-01 echo-05 \
	macro-pass-01 macro-pass-02 macro-pass-03 macro-pass-05 macro-pass-07 \
	macro-pass-08 macro-pass-09 \
	after-02 after-03 \
	spec-02 spec-04 spec-06 spec-07 \
	size-01 size-03 size-04 size-05 \
	weak-01 weak-02 weak-03 \
	spec_min_sep-01 spec_min_sep-03 spec_min_sep-04 spec_min_sep-05 \
	spec_min_sep-06 spec_min_sep-10 spec_min_sep-11 \
	bug-001 \
	top-level-01 top-level-04 \
	flat-type-01 flat-type-02 flat-type-03 \
	lvs-order-01 lvs-order-02 lvs-order-03 \
	lvs-xcpl-inv-01 \
	tree-02 tree-03 tree-04 \
	subckt-01 subckt-05 subckt-06 \
	subckt-07 subckt-08 \
	subckt-09 subckt-10 \
	subckt-11 subckt-12 \
	subckt-13 subckt-14 subckt-15 subckt-16 \
	precharge-00 \
	precharge-02 \
	unstab-01 unstab-02 \
	comb-01 \
	iscomb-01 iscomb-02 iscomb-03 iscomb-04 iscomb-05 iscomb-06 \
	iscomb-07 iscomb-08 iscomb-09 iscomb-10 iscomb-11 iscomb-12 \
	iscomb-13 iscomb-14 iscomb-15 iscomb-16 iscomb-17 iscomb-18 \
	iscomb-19 iscomb-20 iscomb-21 iscomb-22 iscomb-23 iscomb-24 \
	iscomb-25 iscomb-26 iscomb-26-a iscomb-26-b \
	iscomb-27 \
	iscomb-27-a iscomb-27-b iscomb-27-c iscomb-27-d iscomb-27-e \
	iscomb-27-f iscomb-27-g iscomb-27-h iscomb-27-i iscomb-27-j \
	iscomb-27-k iscomb-27-l iscomb-27-m iscomb-27-n iscomb-27-o \
	iscomb-27-p iscomb-27-q \
	keeper-01 \
	iskeeper-01 iskeeper-02 iskeeper-03 iskeeper-04 \
	isckeeper-01 isckeeper-02 \
	autokeeper-01 autokeeper-02 autokeeper-03 autokeeper-04 \
	isrvc-01 \
	interference-01 interference-02 interference-03 \
	internal-node-00 internal-node-06 \
	internal-node-12 internal-node-13 internal-node-14 internal-node-15 \
	internal-node-17 \
	act-001 act-002 act-003 act-004 act-005 act-006 act-007 act-010 \
	connect-01 connect-02 connect-03 \
	auto-celem-00 auto-celem-01 auto-celem-02 auto-celem-03 auto-celem-04 \
	auto-celem-05 auto-celem-06 auto-celem-07 auto-celem-08 auto-celem-09 \
	auto-celem-10 auto-celem-11 auto-celem-12 auto-celem-13 \
	strengths-01 strengths-02 \
	invariant-01 invariant-02 invariant-03 invariant-04 \
	flavor-01 flavor-03 flavor-04 flavor-05 \
	supply-01 supply-02 supply-03 supply-04 supply-05 supply-06 \
	runmodestatic-01 runmodestatic-03 runmodestatic-04 runmodestatic-05 \
	reff-01 \
	load_cap-01 

HACKT_CREATE_TEST_FAILURES = \
	loop-01 loop-02 loop-03 \
	loop-11 loop-12 loop-13 loop-17 \
	null-01 \
	act-009 \
	internal-node-07 internal-node-09 internal-node-10

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
	loop-00 loop-04 loop-05 loop-06 loop-07 loop-08 loop-09 \
	loop-10 loop-14 loop-15 loop-16 loop-18 loop-19 loop-20 \
	cond-01 cond-02 cond-03 cond-04 cond-05 cond-06 cond-07 \
	cond-08 cond-09 cond-10 cond-11 cond-12 cond-13 cond-14 \
	cond-15 cond-16 cond-17 cond-18 \
	short-01 short-02 short-03 short-04 short-05 \
	echo-01 echo-05 \
	macro-pass-01 macro-pass-02 macro-pass-03 macro-pass-05 macro-pass-07 \
	macro-pass-05b macro-pass-07b \
	macro-pass-05c macro-pass-07c \
	after-02 after-03 \
	spec-02 spec-04 spec-06 spec-07 \
	size-01 size-03 size-04 size-05 \
	weak-01 weak-02 weak-03 \
	bug-001 \
	top-level-01 top-level-04 \
	flat-type-01 flat-type-02 flat-type-03 \
	tree-02 tree-03 \
	subckt-01 \
	precharge-02 \
	unstab-01 unstab-02 \
	comb-01 \
	iscomb-01 iscomb-02 iscomb-03 iscomb-04 iscomb-05 iscomb-06 \
	iscomb-07 iscomb-08 iscomb-09 iscomb-10 iscomb-11 iscomb-12 \
	iscomb-13 iscomb-14 iscomb-15 iscomb-16 iscomb-17 iscomb-18 \
	iscomb-19 iscomb-20 iscomb-21 iscomb-22 iscomb-23 iscomb-24 \
	iscomb-25 iscomb-26 iscomb-26-a iscomb-26-b \
	iscomb-27 \
	iscomb-27-a iscomb-27-b iscomb-27-c iscomb-27-d iscomb-27-e \
	iscomb-27-f iscomb-27-g iscomb-27-h iscomb-27-i iscomb-27-j \
	iscomb-27-k iscomb-27-l iscomb-27-m iscomb-27-n iscomb-27-o \
	iscomb-27-p iscomb-27-q \
	keeper-01 \
	iskeeper-01 iskeeper-02 iskeeper-03 iskeeper-04 \
	autokeeper-01 autokeeper-02 autokeeper-03 autokeeper-04 \
	isrvc-01 \
	interference-01 interference-02 interference-03 \
	internal-node-00 internal-node-06 \
	internal-node-12 internal-node-13 internal-node-14 internal-node-15 \
	internal-node-17 \
	strengths-01 strengths-02 \
	invariant-01 invariant-02 invariant-03 invariant-04 \
	connect-01 connect-02 connect-03 \
	auto-celem-00 auto-celem-01 auto-celem-02 auto-celem-03 auto-celem-04 \
	auto-celem-05 auto-celem-06 auto-celem-07 auto-celem-08 auto-celem-09 \
	auto-celem-10 auto-celem-11 auto-celem-12 auto-celem-13 \
	runmodestatic-01 runmodestatic-03 runmodestatic-04 runmodestatic-05 \
	act-001 act-002 act-003 act-004 act-005 act-006 act-007 act-010 \
	reff-01 \
	flavor-01 flavor-03 flavor-04 flavor-05

# TODO: after implementing supply annotation in hflat
#	supply-01 supply-02 supply-03 supply-04 supply-05 supply-06

HACKT_CFLAT_LVS_TEST_PASSES = \
	nand-04 nand-08 nand-09 \
	lvs-order-01 lvs-order-03 \
	lvs-xcpl-inv-01 \
	macro-pass-02 macro-pass-03 macro-pass-05 macro-pass-07 \
	macro-pass-05b macro-pass-07b \
	weak-03 \
	invariant-01 invariant-02 invariant-03 invariant-04 \
	iscomb-01 iscomb-02 iscomb-03 iscomb-04 iscomb-05 iscomb-06 \
	iscomb-07 iscomb-08 iscomb-09 iscomb-10 iscomb-11 iscomb-12 \
	iscomb-13 iscomb-14 iscomb-15 iscomb-16 iscomb-17 iscomb-18 \
	iscomb-19 iscomb-20 iscomb-21 iscomb-22 iscomb-23 iscomb-24 \
	iscomb-25 iscomb-26 iscomb-26-a iscomb-26-b \
	iscomb-27 \
	iscomb-27-a iscomb-27-b iscomb-27-c iscomb-27-d iscomb-27-e \
	iscomb-27-f iscomb-27-g iscomb-27-h iscomb-27-i iscomb-27-j \
	iscomb-27-k iscomb-27-l iscomb-27-m iscomb-27-n iscomb-27-o \
	iscomb-27-p iscomb-27-q \
	autokeeper-01 autokeeper-02 autokeeper-03 autokeeper-04 \
	runmodestatic-01 runmodestatic-03 runmodestatic-04 runmodestatic-05 \
	isrvc-01 \
	iskeeper-01 iskeeper-02 iskeeper-03 iskeeper-04 \
	isckeeper-01 isckeeper-02

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
	strengths-01 strengths-02 \
	macro-pass-03 macro-pass-05 macro-pass-07 \
	act-005 act-010
# pending: act-004

# subset of HACKT_CFLAT_TEST_SUBJECTS
HACKT_PRSIMEXPR_TEST_SUBJECTS = inv-14 \
	celem-01 \
	short-01 short-02 short-03 short-04 short-05

# expecting empty .prs
EMPTY_CFLAT_PRS_TESTS += \
	cond-08 cond-10 cond-11 cond-15 \
	macro-pass-01

# more output expectation files for when bison is used
BISON_OUTPUTS += inv-01 inv-11 \
	echo-02 after-01 size-02 \
	spec_min_sep-07 spec_min_sep-08 spec_min_sep-09 \
	top-level-03 \
	precharge-01 \
	internal-node-05 \
	flavor-02 \
	supply-11 supply-12

# includes other sources files used by test-cases (but aren't cases themselves)
srcdir_EXTRA_DIST += celem.hac nand.hac nor.hac logic.hac

include $(top_srcdir)/test/Make.test-tail

