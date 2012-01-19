# "test/net/Makefile.am"
#	$Id: Makefile.am,v 1.33 2011/04/29 18:37:11 fang Exp $
# Test auto-Makefile for hacknet.  

include $(top_srcdir)/test/Make.test-head

SUBDIRS = . verilog hac

# .hac inputs
HACKNET_TEST_SCRIPTS_PASSES = \
	inv-00 inv-01 inv-02 inv-03 inv-04 inv-05 inv-06 inv-07 \
	inv-08 inv-09 inv-10 inv-11 inv-12 \
	inv-13 inv-14 inv-15 inv-16 \
	no-print \
	spectre-inv-01 spectre-inv-02 spectre-inv-04 spectre-inv-08 \
	spectre-inv-13 \
	nand-01 nand-02 nand-03 \
	celem-01 celem-02 celem-03 celem-04 celem-05 celem-06 \
	celem-07 celem-08 celem-09 celem-10 celem-11 \
	logic-01 logic-02 \
	ladder-01 ladder-02 \
	mux-01 mux-02 mux-03 mux-04 mux-05 mux-06 \
	port-alias-01 port-alias-02 port-alias-03 port-alias-04 port-alias-05 \
	top-global-params-01 top-global-params-02 \
	top-global-params-03 top-global-params-04 \
	unused-port-01 \
	unconnected-01 unconnected-02 \
	internal-00 internal-01 internal-02 internal-03 internal-04 \
	internal-05 internal-06 internal-07 internal-08 \
	internal-09 internal-10 internal-11 internal-12 \
	internal-13 \
	precharge-01 precharge-02 precharge-03 precharge-04 precharge-05 \
	precharge-06 precharge-07 precharge-08 \
	precharge-internal-01 precharge-internal-02 precharge-internal-03 \
	non-CMOS-01 non-CMOS-02 \
	non-CMOS-07 non-CMOS-08 \
	non-CMOS-09 non-CMOS-10 non-CMOS-11 non-CMOS-12 \
	sizing-01 sizing-02 sizing-03 sizing-04 sizing-05 \
	sizing-06 sizing-07 sizing-08 sizing-09 \
	half-pull-01 half-pull-02 \
	pass_gate-01 pass_gate-02 pass_gate-03 pass_gate-04 \
	pass_gate-05 pass_gate-06 pass_gate-07 \
	spectre-pass_gate-02 \
	subckt-01 subckt-03 subckt-04 subckt-05 subckt-06 subckt-07 subckt-08 \
	subckt-09 subckt-10 subckt-11 \
	spectre-subckt-01 spectre-subckt-03 \
	channel-member-01 channel-member-02 channel-member-03 \
	array-01 \
	iskeeper-01 iskeeper-02 \
	process-port-01 process-port-02 \
	process-port-03 process-port-04 \
	process-port-05 process-port-06 \
	process-port-07 process-port-08 \
	channel-port-01 channel-port-02 \
	empty-01 empty-02 empty-03 empty-04 \
	recurse-01 recurse-02 recurse-03 recurse-04 \
	config-dump-01 \
	config-arg-01 config-arg-02 config-arg-03 config-arg-04 \
	config-arg-06 config-arg-08 config-arg-09 \
	config-file-01 config-file-02 config-file-03 \
	config-file-05 config-file-06 config-file-07 config-file-08 \
	compat-01 compat-02 compat-03 \
	tsmc65 tsmc65-equiv \
	fet-type-01 fet-type-02 fet-type-03 fet-type-04 \
	fet-type-05 fet-type-06 fet-type-07 fet-type-08 \
	fet-type-09 fet-type-10 fet-type-11 fet-type-12 \
	fet-type-13 fet-type-14 fet-type-15 fet-type-16 \
	fet-type-17 fet-type-18 fet-type-19 fet-type-20 \
	fet-type-21 fet-type-22 fet-type-23 fet-type-24 \
	fet-type-25 fet-type-26 fet-type-27 fet-type-28 \
	fet-type-29 fet-type-30 fet-type-31 fet-type-32 \
	fet-type-33 fet-type-34 fet-type-35 fet-type-36 \
	parasitics-01 \
	aliases-01 aliases-02 aliases-03 aliases-04 \
	node_caps-01 node_caps-02 node_caps-03 node_caps-04 \
	node_caps-05 node_caps-06 \
	mangle-01 mangle-02 mangle-03 mangle-04 mangle-04b mangle-05 \
	mangle-06 mangle-07 mangle-08 mangle-09 \
	quoted-names-01 quoted-names-02 \
	struct-01 struct-02 struct-03 struct-04 struct-05 struct-06 \
	supply-01 supply-02 supply-03 supply-04 supply-05 supply-06 \
	supply-07 supply-08 \
	supply-subckt-01 supply-subckt-02 supply-subckt-03 \
	supply-subckt-04 supply-subckt-05 \
	spectre-supply-subckt-01 \
	supply-internal-01 supply-internal-02 supply-internal-03 \
	supply-precharge-01 supply-precharge-02 supply-precharge-03 \
	supply-precharge-04 \
	spectre-precharge-04 \
	supply-pass-gate-01 supply-pass-gate-02 \
	supply-instance-01 supply-instance-02 supply-instance-03 \
	supply-instance-04 supply-instance-05 supply-instance-06 \
	supply-instance-07 supply-instance-08 supply-instance-09 \
	supply-instance-10 supply-instance-11 supply-instance-12 \
	supply-instance-13 supply-instance-14 supply-instance-15 \
	supply-instance-16 supply-instance-17 supply-instance-18 \
	supply-instance-02b supply-instance-11b \
	spectre-supply-instance-01 \
	substrate-01 substrate-02 substrate-03 \
	substrate-04 substrate-05 substrate-06 \
	spectre-substrate-05 \
	rename-01 rename-02 rename-03 rename-06 \
	rename-07 rename-08 rename-09 rename-10 rename-11 rename-12 \
	rename-14 rename-15 \
	reserved-01 reserved-02 reserved-03 reserved-04 \
	preferred_names-01 preferred_names-02 preferred_names-03 \
	preferred_names-04 preferred_names-05 preferred_names-06 \
	terminal-test-01 terminal-test-02 terminal-test-03 terminal-test-04 \
	terminal-test-05 terminal-test-06 terminal-test-07 terminal-test-08 \
	terminal-test-09 terminal-test-10 terminal-test-11 terminal-test-12 \
	terminal-test-13 terminal-test-14 terminal-test-15 terminal-test-16 \
	terminal-test-17 terminal-test-18

# TODO: implement string manipulation to modify rename-04,05

HACKNET_TEST_SCRIPTS_FAILURES = \
	non-CMOS-00 non-CMOS-03 non-CMOS-04 non-CMOS-05 \
	config-arg-07 config-arg-10 config-file-04 \
	supply-internal-04 \
	case-01 case-02 case-03 case-04 \
	reserved-05 reserved-06 \
	rename-04 rename-05 \
	type-mangle-collision-01 type-mangle-collision-02 \
	iskeeper-01b \
	sizing-08b

# TODO: promote these to real tests
EXTRA_DIST_HAC = \
	globals \
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
	non-CMOS-06 subckt-02 subckt-08 \
	fet-type-01 fet-type-02 fet-type-03 fet-type-04 \
	fet-type-05 fet-type-06 fet-type-07 fet-type-08 \
	fet-type-18 fet-type-19 fet-type-20 fet-type-21 \
	fet-type-22 fet-type-23 fet-type-24 fet-type-25 \
	fet-type-26 fet-type-27 fet-type-28 \
	fet-type-29 fet-type-30 fet-type-31 fet-type-32 \
	fet-type-33 fet-type-34 fet-type-35 fet-type-36 \
	pass_gate-05 pass_gate-06 \
	precharge-07 precharge-08 \
	mangle-04 \
	rule-sizes-01 rule-sizes-02 rule-sizes-03 rule-sizes-04 \
	rule-sizes-05 rule-sizes-06 \
	rename-14

HACKT_PARSE_TEST_FAILURES = \
	rename-13

CLEANFILES +=
AUTO_IGNORE +=
EXTRA_DIST +=

HACKT_CREATE_TEST_PASSES = \
	fet-type-01 fet-type-02 fet-type-03 fet-type-04 \
	fet-type-05 fet-type-06 fet-type-07 fet-type-08 \
	fet-type-18 fet-type-19 fet-type-20 fet-type-21 \
	fet-type-22 fet-type-23 fet-type-24 fet-type-25 \
	fet-type-26 fet-type-27 fet-type-28 \
	fet-type-29 fet-type-30 fet-type-31 fet-type-32 \
	fet-type-33 fet-type-34 fet-type-35 fet-type-36 \
	pass_gate-05 pass_gate-06 \
	precharge-07 precharge-08 \
	mangle-04 \
	subckt-01 subckt-08 subckt-10 subckt-11 \
	supply-pass-gate-01 supply-pass-gate-02 \
	supply-instance-01 supply-instance-02 supply-instance-03 \
	supply-instance-04 supply-instance-05 supply-instance-06 \
	supply-instance-07 supply-instance-08 supply-instance-09 \
	supply-instance-10 supply-instance-11 supply-instance-12 \
	supply-instance-13 supply-instance-14 supply-instance-15 \
	supply-instance-16 supply-instance-17 supply-instance-18 \
	supply-instance-02b supply-instance-11b \
	substrate-01 substrate-02 substrate-03 \
	substrate-04 substrate-05 substrate-06 \
	rule-sizes-01 rule-sizes-02 rule-sizes-03 rule-sizes-04 \
	rule-sizes-05 rule-sizes-06 \
	rename-14

HACKT_CREATE_TEST_FAILURES = \
	non-CMOS-06 subckt-02

HACKT_ALLOC_TEST_PASSES = \
	supply-instance-01 supply-instance-02 supply-instance-03 \
	supply-instance-04 supply-instance-05 supply-instance-06 \
	supply-instance-07 supply-instance-08 supply-instance-09 \
	supply-instance-10 supply-instance-11 supply-instance-12 \
	supply-instance-13 supply-instance-14 supply-instance-15 \
	supply-instance-16 supply-instance-17 supply-instance-18 \
	supply-instance-02b supply-instance-11b

HACKT_CFLAT_TEST_SUBJECTS =

HACKT_CFLAT_SPRS_TEST_SUBJECTS = \
	rule-sizes-01 rule-sizes-02 rule-sizes-03 rule-sizes-04 \
	rule-sizes-05 rule-sizes-06

include $(top_srcdir)/test/Make.test-tail

