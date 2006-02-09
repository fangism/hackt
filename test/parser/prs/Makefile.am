# "test/parser/prs/Makefile.am"
#	$Id: Makefile.am,v 1.13.2.1 2006/02/09 00:35:20 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = inv-01 inv-04 inv-05 inv-10 inv-11 \
	celem-07 celem-08 \
	echo-02 echo-03 echo-04 \
	after-01 \
	spec-01 spec-03 \
	macro-param-01 macro-param-02 macro-param-03

HACKT_PARSE_TEST_PASSES = inv-02 inv-03 inv-06 inv-07 inv-08 inv-09 \
	inv-12 inv-13 inv-14 inv-15 inv-16 inv-17 \
	nand-01 nand-02 nand-03 nand-04 nand-05 nand-06 nand-07 \
	nor-01 xor-01 \
	celem-01 celem-02 celem-03 celem-04 celem-05 celem-06 \
	wchb-01 wchb-02 wchb-03 wchb-04 \
	null-00 null-01 \
	pal-01 pal-02 pal-03 \
	loop-00 loop-01 loop-02 loop-03 \
	short-01 short-02 short-03 short-04 short-05 \
	echo-01 \
	macro-pass-01 macro-pass-02 \
	after-02 after-03 \
	spec-02 spec-04

HACKT_PARSE_TEST_SUBJECTS = $(HACKT_PARSE_TEST_FAILURES) $(HACKT_PARSE_TEST_PASSES)

HACKT_OBJ_TEST_SUBJECTS = $(HACKT_PARSE_TEST_PASSES)

HACKT_UNROLL_TEST_SUBJECTS = $(HACKT_OBJ_TEST_SUBJECTS)

HACKT_CREATE_TEST_SUBJECTS = inv-14 inv-15 inv-16 inv-17 \
	celem-01 celem-02 celem-03 celem-04 celem-05 celem-06 \
	nand-04 nand-05 nand-06 nand-07 \
	wchb-01 wchb-02 wchb-03 wchb-04 \
	null-00 null-01 \
	pal-01 pal-02 pal-03 \
	loop-00 loop-01 loop-02 loop-03 \
	short-01 short-02 short-03 short-04 short-05 \
	echo-01 \
	macro-pass-01 macro-pass-02 \
	after-02 after-03 \
	spec-02 spec-04

# known defect reports: unresolved language issues
# dealing with empty loops and negative ranges...
# null-01: waiting for null-expression resolution
# loop-01: (same)

# HACKT_ALLOC_TEST_SUBJECTS = $(HACKT_CREATE_TEST_SUBJECTS)
HACKT_ALLOC_TEST_SUBJECTS = inv-14 inv-15 inv-16 inv-17 \
	celem-01 celem-02 celem-03 celem-04 celem-05 celem-06 \
	nand-04 nand-05 nand-06 nand-07 \
	wchb-01 wchb-02 wchb-03 wchb-04 \
	null-00 \
	pal-01 pal-02 pal-03 \
	loop-00 \
	short-01 short-02 short-03 short-04 short-05 \
	echo-01 \
	macro-pass-01 macro-pass-02 \
	after-02 after-03 \
	spec-02 spec-04

HACKT_CFLAT_TEST_SUBJECTS = $(HACKT_ALLOC_TEST_SUBJECTS)

# subset of HACKT_CFLAT_TEST_SUBJECTS
HACKT_PRSIMEXPR_TEST_SUBJECTS = inv-14 \
	celem-01 \
	short-01 short-02 short-03 short-04 short-05

# expecting empty .prs
EMPTY_CFLAT_PRS_TESTS += null-00 loop-00 \
	macro-pass-01

# more output expectation files for when bison is used
BISON_OUTPUTS += inv-01 inv-11 \
	echo-02 after-01

# includes other sources files used by test-cases (but aren't cases themselves)
EXTRA_DIST += celem.hac nand.hac nor.hac logic.hac

# TODO: haven't gotten around to these yet...
EXTRA_DIST += loop-04.hac loop-05.hac cond-01.hac \
	size-01.hac size-02.hac size-03.hac

include $(top_srcdir)/test/Make.test-tail

