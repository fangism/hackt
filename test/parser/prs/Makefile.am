# "test/parser/prs/Makefile.am"
#	$Id: Makefile.am,v 1.5.2.2 2005/10/11 02:41:29 fang Exp $

include $(top_srcdir)/test/Make.test-head

ARTC_TEST_FAILURES = inv-01 inv-04 inv-05 inv-10 inv-11

ARTC_TEST_PASSES = inv-02 inv-03 inv-06 inv-07 inv-08 inv-09 \
	inv-12 inv-13 inv-14 inv-15 inv-16 inv-17 \
	nand-01 nand-02 nand-03 nor-01 xor-01 \
	celem-01 celem-02 celem-03 celem-04 celem-05 \
	wchb-01 wchb-02 wchb-03 wchb-04

# celem-06 pending parenthesization fix

ARTC_TEST_SUBJECTS = $(ARTC_TEST_FAILURES) $(ARTC_TEST_PASSES)

ARTOBJ_TEST_SUBJECTS = $(ARTC_TEST_PASSES)

ARTOBJUNROLL_TEST_SUBJECTS = $(ARTOBJ_TEST_SUBJECTS)

HACKT_CREATE_TEST_SUBJECTS = inv-14 inv-15 inv-16 inv-17 \
	celem-01 celem-02 celem-03 celem-04 celem-05 \
	wchb-01 wchb-02 wchb-03 wchb-04

HACKT_ALLOC_TEST_SUBJECTS = $(HACKT_CREATE_TEST_SUBJECTS)

HACKT_CFLAT_TEST_SUBJECTS = $(HACKT_ALLOC_TEST_SUBJECTS)

# more output expectation files for when bison is used
BISON_OUTPUTS += inv-01 inv-11

include $(top_srcdir)/test/Make.test-tail

