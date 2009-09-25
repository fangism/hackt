# "test/net/Makefile.am"
#	$Id: Makefile.am,v 1.3.2.1 2009/09/25 01:21:43 fang Exp $
# Test auto-Makefile for hacknet.  

include $(top_srcdir)/test/Make.test-head

# SUBDIRS =

# .hac inputs
HACKNET_TEST_SCRIPTS_PASSES = \
	inv-00 inv-01 inv-02 inv-03 inv-04 inv-05 inv-06 inv-07 \
	inv-08 inv-09 inv-10 inv-11 inv-12 \
	nand-01 nand-02 nand-03 \
	celem-01 celem-02 celem-03 celem-04 celem-05 celem-06 \
	celem-07 celem-08 \
	port-alias-01 port-alias-02 port-alias-03 port-alias-04 port-alias-05 \
	unused-port-01 \
	unconnected-01 unconnected-02 \
	internal-00 internal-01 internal-02 internal-03 internal-04 \
	internal-05 internal-06 internal-07 internal-08 \
	internal-09 internal-10 internal-11 internal-12 \
	precharge-01 precharge-02 precharge-03 precharge-04 precharge-05 \
	precharge-06 precharge-07 precharge-08 \
	precharge-internal-01 precharge-internal-02 precharge-internal-03 \
	non-CMOS-01 non-CMOS-02 \
	sizing-01 sizing-02 sizing-03 sizing-04 sizing-05 \
	sizing-06 sizing-07 sizing-08 \
	half-pull-01 half-pull-02 \
	pass_gate-01 pass_gate-02 pass_gate-03 pass_gate-04 \
	pass_gate-05 pass_gate-06 \
	subckt-01 subckt-03 \
	channel-member-01 channel-member-02 channel-member-03 \
	array-01 \
	iskeeper-01 iskeeper-02 \
	process-port-01 process-port-02 \
	process-port-03 process-port-04 \
	process-port-05 process-port-06 \
	process-port-07 process-port-08 \
	empty-01 empty-02 empty-03 \
	recurse-01 recurse-02 \
	config-dump-01 \
	config-arg-01 config-arg-02 config-arg-03 config-arg-04 \
	config-arg-06 config-arg-08 \
	config-file-01 config-file-02 config-file-03 \
	config-file-05 config-file-06 config-file-07 config-file-08 \
	compat-01 compat-02 compat-03 \
	tsmc65 tsmc65-equiv \
	fet-type-01 fet-type-02 fet-type-03 fet-type-04 \
	fet-type-05 fet-type-06 fet-type-07 fet-type-08 \
	parasitics-01 \
	mangle-01 mangle-02 mangle-03 mangle-04 mangle-04b mangle-05 \
	mangle-06 mangle-07 mangle-08 mangle-09 \
	struct-01 struct-02 struct-03 struct-04 struct-05 struct-06 \
	supply-01 supply-02 supply-03 supply-04 supply-05 supply-06
	supply-subckt-01 supply-subckt-02 supply-subckt-03

HACKNET_TEST_SCRIPTS_FAILURES = \
	non-CMOS-00 non-CMOS-03 non-CMOS-04 non-CMOS-05 \
	config-arg-07 config-file-04

# TODO: promote these to real tests
EXTRA_DIST_HAC = \
	config-arg-05 \
	top-type-ports-01

EXTRA_DIST += $(EXTRA_DIST_HAC:=.hac)

srcdir_EXTRA_DIST += \
	sample-01.hacknet-conf \
	sample-02.hacknet-conf \
	sample-03.hacknet-conf \
	sample-04.hacknet-conf \
	netgen-defaults.hacknet-conf \
	mangle-example-01.hacknet-conf \
	mangle-example-02.hacknet-conf \
	tsmc65.netgen-conf \
	tsmc65-equiv.netgen-conf

HACKT_PARSE_TEST_PASSES = \
	non-CMOS-06 subckt-02 \
	fet-type-01 fet-type-02 fet-type-03 fet-type-04 \
	fet-type-05 fet-type-06 fet-type-07 fet-type-08 \
	pass_gate-05 pass_gate-06 \
	precharge-07 precharge-08 \
	mangle-04

HACKTNET_TEST_SCRIPTS_PASSES =

CLEANFILES +=
AUTO_IGNORE +=
EXTRA_DIST +=

HACKT_CREATE_TEST_PASSES = \
	fet-type-01 fet-type-02 fet-type-03 fet-type-04 \
	fet-type-05 fet-type-06 fet-type-07 fet-type-08 \
	pass_gate-05 pass_gate-06 \
	precharge-07 precharge-08 \
	mangle-04
HACKT_CREATE_TEST_FAILURES = \
	non-CMOS-06 subckt-02
HACKT_CFLAT_TEST_SUBJECTS =

include $(top_srcdir)/test/Make.test-tail

