# "test/net/Makefile.am"
#	$Id: Makefile.am,v 1.1.2.6 2009/08/13 17:16:29 fang Exp $
# Test auto-Makefile for hacknet.  

include $(top_srcdir)/test/Make.test-head

# SUBDIRS =

# .hac inputs
HACKNET_TEST_SCRIPTS_PASSES = \
	inv-00 inv-01 inv-02 inv-03 inv-04 inv-05 inv-06 inv-07 \
	nand-01 nand-02 nand-03 \
	celem-01 celem-02 celem-03 celem-04 celem-05 celem-06
# TODO: promote these to real tests
EXTRA_DIST_HAC = \
	celem-05 celem-06 \
	subckt-01 subckt-02 \
	internal-00 internal-01 internal-02 internal-03 internal-04 \
	non-CMOS-01 non-CMOS-02 non-CMOS-03 \
	port-alias-01 port-alias-02

EXTRA_DIST += $(EXTRA_DIST_HAC:=.hac)

HACKT_PARSE_TEST_PASSES =

HACKTNET_TEST_SCRIPTS_PASSES =

CLEANFILES +=
AUTO_IGNORE +=
EXTRA_DIST +=

HACKT_CREATE_TEST_PASSES =
HACKT_CREATE_TEST_FAILURES =
HACKT_CFLAT_TEST_SUBJECTS =

HACKNET_TEST_SCRIPTS_FAILURES =

srcdir_EXTRA_DIST +=

include $(top_srcdir)/test/Make.test-tail

