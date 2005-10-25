# "test/parser/prs/Makefile.am"
#	$Id: Makefile.am,v 1.5.2.7 2005/10/25 04:19:01 fang Exp $

include $(top_srcdir)/test/Make.test-head

ARTC_TEST_FAILURES = inv-01 inv-04 inv-05 inv-10 inv-11 \
	celem-07 celem-08

ARTC_TEST_PASSES = inv-02 inv-03 inv-06 inv-07 inv-08 inv-09 \
	inv-12 inv-13 inv-14 inv-15 inv-16 inv-17 \
	nand-01 nand-02 nand-03 nand-04 nand-05 nand-06 nand-07 \
	nor-01 xor-01 \
	celem-01 celem-02 celem-03 celem-04 celem-05 celem-06 \
	wchb-01 wchb-02 wchb-03 wchb-04 \
	null-00 null-01 \
	pal-01 pal-02 pal-03 \
	loop-00 loop-01 loop-02 loop-03

ARTC_TEST_SUBJECTS = $(ARTC_TEST_FAILURES) $(ARTC_TEST_PASSES)

ARTOBJ_TEST_SUBJECTS = $(ARTC_TEST_PASSES)

ARTOBJUNROLL_TEST_SUBJECTS = $(ARTOBJ_TEST_SUBJECTS)

HACKT_CREATE_TEST_SUBJECTS = inv-14 inv-15 inv-16 inv-17 \
	celem-01 celem-02 celem-03 celem-04 celem-05 celem-06 \
	nand-04 nand-05 nand-06 nand-07 \
	wchb-01 wchb-02 wchb-03 wchb-04 \
	null-00 null-01 \
	pal-01 pal-02 pal-03 \
	loop-00 loop-01 loop-02 loop-03

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
	loop-00

HACKT_CFLAT_TEST_SUBJECTS = $(HACKT_ALLOC_TEST_SUBJECTS)

EMPTY_CFLAT_PRS_TESTS = null-00 loop-00
EMPTY_CFLAT_PRS = $(EMPTY_CFLAT_PRS_TESTS:=.expect-prs)

$(EMPTY_CFLAT_PRS):
	touch $@

all-local: $(EMPTY_CFLAT_PRS)

AUTO_IGNORE += $(EMPTY_CFLAT_PRS)
CLEANFILES += $(EMPTY_CFLAT_PRS)

# more output expectation files for when bison is used
BISON_OUTPUTS += inv-01 inv-11

include $(top_srcdir)/test/Make.test-tail

