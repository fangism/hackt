# "test/prsim/Makefile.am"
#	$Id: Makefile.am,v 1.21.4.7 2007/12/28 18:44:59 fang Exp $
# Test auto-Makefile for prsim.  

include $(top_srcdir)/test/Make.test-head

# .hac inputs
HACKT_PRSIMRC_TEST_SUBJECTS = \
	celem inv oscillator half-celem deep tree \
	celem-share after-celem-def after-celem after-celem-more \
	pass-gates-def pass-gates excl-rings-def excl-rings \
	check-excl-rings-def check-excl-rings \
	logic-defs and nand or nor array \
	expr-limit-a \
	precharge-a precharge-b \
	weak-fight-a \
	weak-vs-weak \
	weak-feedback-a weak-feedback-b \
	celem-fb \
	half-keeper-a \
	cross-coupled cross-coupled-weak \
	cbit_test_stat \
	cross-pull-a \
	bitline-a bitline-b

# TODO: more cross-coupled test cases
# SRAM cell
# register with read/write ports, cross-coupled

HACKT_PARSE_TEST_PASSES = \
	expr-limit-b

HACKT_PRSIMRC_TEST_SCRIPTS_PASSES = \
	celem-01 celem-02 celem-03 celem-04 \
	inv-00 inv-01 inv-02 inv-03 inv-04 inv-05 inv-07 inv-08 inv-09 \
	inv-11 inv-12 inv-13 inv-14 inv-15 inv-16 \
	and-01 nand-01 nand-02 or-01 nor-01 \
	expr-limit-a-01 \
	unstable-01 unstable-02 unstable-03 \
	oscil-break-01 oscil-break-02 oscil-break-03 \
	oscil-random-01 oscil-what-01 oscil-what-02 \
	oscil-unset-01 oscil-unset-02 oscil-unset-03 \
	oscil-backtrace-01 \
	half-celem-01 \
	celem-share-01 celem-share-02 \
	deep-01 deep-02 \
	tree-01 tree-02 \
	array-01 \
	aliases-08 aliases-09 \
	timing-01 \
	after-celem-base \
	after-celem-01 after-celem-02 after-celem-03 \
	pass-gates-01 pass-gates-02 pass-gates-03 \
	excl-rings-01 excl-rings-02 excl-rings-03 \
	check-excl-rings-02 check-excl-rings-04 check-excl-rings-05 \
	precharge-a-01 precharge-b-01 \
	weak-fight-a-01 weak-fight-a-03 weak-fight-a-04 \
	weak-vs-weak-01 weak-vs-weak-02 weak-vs-weak-03 \
	weak-vs-weak-04 \
	weak-feedback-a-01 weak-feedback-b-01 \
	celem-fb-01 \
	half-keeper-a-01 half-keeper-a-02 \
	cross-coupled-01 cross-coupled-weak-01 \
	cbit_test_stat-01 \
	cross-pull-a-01 cross-pull-a-02 \
	bitline-a-01 bitline-a-02 \
	bitline-b-01 bitline-b-02

$(srcdir)/cbit_test_stat-01.prsimrc: gen_cbit_test.awk
	{ $(ECHO) "# $@" ; \
	$(ECHO) "# @haco@ cbit_test_stat.haco-a" ; \
	$(AWK) -f $< ;} > $@

# extra dep
cbit_test_stat-01.prsimrc-out: cbit_test_stat.haco-a

CLEANFILES += cbit_test_stat-01.prsimrc
AUTO_IGNORE += cbit_test_stat-01.prsimrc
EXTRA_DIST += cbit_test_stat-01.prsimrc gen_cbit_test.awk

# all weak-* tests added (pass AND fail) are consistent with Rajit's prsim
# Fail tests (below) need to be revisited to "do the right thing".

# NOTE: expr-limit-a-01 should fail before limit was increased beyond 255

HACKT_CREATE_TEST_FAILURES = \
	expr-limit-b

HACKT_PRSIMRC_TEST_SCRIPTS_FAILURES = \
	inv-06 inv-10 \
	cycle-source-a cycle-source-b \
	aliases-01 aliases-02 aliases-03 aliases-04 \
	aliases-05 aliases-06 aliases-07 \
	check-excl-rings-01 check-excl-rings-03 \
	weak-fight-a-02 weak-fight-a-05 \
	weak-feedback-a-02 weak-feedback-b-02 \
	celem-fb-02

HACKT_PRSIM_CKPT_TEST_SCRIPTS_BASES = $(HACKT_PRSIMRC_TEST_SCRIPTS_PASSES)

HACKT_PRSIM_OPT_TEST_SCRIPTS_BASES = $(HACKT_PRSIM_CKPT_TEST_SCRIPTS_BASES)

srcdir_EXTRA_DIST += array-b.hac
# array-b.hac is pending BUG fix
srcdir_EXTRA_DIST += half-keeper-cycle.prsimrc

include $(top_srcdir)/test/Make.test-tail

