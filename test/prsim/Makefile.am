# "test/prsim/Makefile.am"
#	$Id: Makefile.am,v 1.21.2.21 2008/02/06 06:24:50 fang Exp $
# Test auto-Makefile for prsim.  

include $(top_srcdir)/test/Make.test-head

# .hac inputs
HACKT_PRSIMRC_TEST_SUBJECTS = \
	celem inv oscillator half-celem deep tree interference \
	interference-a inv-alt unused \
	celem-share after-celem-def after-celem after-celem-more \
	pass-gates-def pass-gates excl-rings-def excl-rings \
	check-excl-rings-def check-excl-rings \
	logic-defs and nand or nor array \
	expr-limit-a \
	unstable-a unstable-b \
	weak-unstable-a weak-unstable-b \
	precharge-a precharge-b precharge-c precharge-d \
	precharge-e precharge-f \
	weak-inv \
	weak-fight-a weak-fight-b \
	weak-vs-weak \
	weak-feedback-a weak-feedback-b \
	weak-yield-a weak-yield-b \
	celem-fb \
	half-weak-a half-weak-b \
	half-keeper-a \
	cross-coupled cross-coupled-weak \
	cbit_test_stat \
	cross-pull-a \
	bitline-a bitline-b \
	SRAM SRAM-a SRAM-b SRAM-c SRAM-d \
	keeper-a

# TODO: more cross-coupled test cases
# SRAM cell
# register with read/write ports, cross-coupled

HACKT_PARSE_TEST_PASSES = \
	expr-limit-b

HACKT_PRSIMRC_TEST_SCRIPTS_PASSES = \
	comment-01 comment-02 comment-04 comment-06 \
	celem-01 celem-02 celem-03 celem-04 \
	inv-00 inv-01 inv-02 inv-03 inv-04 inv-05 inv-07 inv-08 inv-09 \
	inv-11 inv-12 inv-13 inv-14 inv-15 inv-16 inv-17 \
	inv-18 inv-19 inv-20 inv-20a inv-21 inv-21a inv-22 inv-23 \
	inv-24 inv-25 inv-26 inv-27 \
	inv-alt-01 inv-alt-02 inv-alt-03 inv-alt-04 inv-alt-05 inv-alt-06 \
	inv-alt-07 inv-alt-08 inv-alt-09 inv-alt-10 \
	and-01 nand-01 nand-02 or-01 nor-01 nor-02 nor-04 \
	unused-01 \
	expr-limit-a-01 \
	interference-01 interference-02 interference-03 interference-04 \
	interference-05 interference-06 interference-07 interference-08 \
	interference-09 interference-10 \
	interference-a-01 interference-a-02 \
	killed-01 \
	unstable-01 unstable-02 unstable-03 \
	unstable-a-01 unstable-b-01 \
	weak-unstable-a-01 weak-unstable-a-02 weak-unstable-a-03 \
	weak-unstable-b-01 weak-unstable-b-02 weak-unstable-b-03 \
	weak-yield-a-01 weak-yield-b-01 \
	oscil-break-01 oscil-break-02 oscil-break-03 \
	oscil-random-01 oscil-random-01a \
	oscil-random-02 oscil-random-02a oscil-random-02b oscil-random-02c \
	oscil-what-01 oscil-what-02 \
	oscil-unset-01 oscil-unset-02 oscil-unset-03 \
	oscil-backtrace-01 \
	oscil-status-01 \
	half-celem-01 \
	celem-share-01 celem-share-02 \
	deep-01 deep-02 \
	tree-01 tree-02 \
	array-01 \
	aliases-08 aliases-09 \
	timing-01 \
	pending-01 pending-02 \
	after-celem-base \
	after-celem-01 after-celem-02 after-celem-03 \
	pass-gates-01 pass-gates-02 pass-gates-03 \
	excl-rings-01 excl-rings-02 excl-rings-03 \
	check-excl-rings-02 check-excl-rings-04 check-excl-rings-05 \
	precharge-a-01 precharge-b-01 \
	precharge-c-01 precharge-d-01 \
	precharge-e-01 precharge-f-01 \
	weak-inv-01 weak-inv-02 \
	weak-fight-a-01 weak-fight-a-02 weak-fight-a-03 weak-fight-a-04 \
	weak-fight-a-05 \
	weak-fight-b-01 \
	weak-vs-weak-01 weak-vs-weak-02 weak-vs-weak-03 \
	weak-vs-weak-04 \
	weak-feedback-a-01 weak-feedback-b-01 \
	celem-fb-01 \
	half-keeper-a-01 half-keeper-a-02 \
	cross-coupled-01 cross-coupled-weak-01 \
	cbit_test_stat-01 \
	cross-pull-a-01 cross-pull-a-02 \
	bitline-a-01 bitline-a-02 \
	bitline-b-01 bitline-b-02 \
	half-weak-a-01 half-weak-a-01a half-weak-a-02 half-weak-a-02a \
	half-weak-a-03 half-weak-a-03a half-weak-a-04 half-weak-a-04a \
	half-weak-b-01 half-weak-b-01a half-weak-b-02 half-weak-b-02a \
	half-weak-b-03 half-weak-b-03a half-weak-b-04 half-weak-b-04a \
	SRAM-a-01 SRAM-a-02 SRAM-a-03 SRAM-a-04 SRAM-a-05 SRAM-a-06 \
	SRAM-b-01 SRAM-b-02 SRAM-b-03 \
	SRAM-c-01 SRAM-c-02 SRAM-c-03 \
	SRAM-d-01 SRAM-d-02 \
	keeper-a-01

# positive different test:
# oscil-random-01 and oscil-random-01a are *supposed* to differ (seeds)

# different custom test: just compare two existing output dumps (unfiltered!)
oscil-random-02c.prsimrc-diff: \
		oscil-random-02a.prsimrc-out oscil-random-02b.prsimrc-out
	$(DIFF) oscil-random-02a.prsimrc-out oscil-random-02b.prsimrc-out > $@

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
	comment-03 comment-05 comment-07 \
	celem-05 \
	inv-06 inv-10 nor-03 \
	cycle-source-a cycle-source-b \
	aliases-01 aliases-02 aliases-03 aliases-04 \
	aliases-05 aliases-06 aliases-07 \
	check-excl-rings-01 check-excl-rings-03 \
	weak-feedback-a-02 weak-feedback-b-02 \
	celem-fb-02

HACKT_PRSIM_CKPT_TEST_SCRIPTS_BASES = $(HACKT_PRSIMRC_TEST_SCRIPTS_PASSES)

HACKT_PRSIM_OPT_TEST_SCRIPTS_BASES = $(HACKT_PRSIM_CKPT_TEST_SCRIPTS_BASES)

srcdir_EXTRA_DIST += array-b.hac
# array-b.hac is pending BUG fix
srcdir_EXTRA_DIST += \
	half-keeper-cycle.prsimrc \
	precharge-c-repeat.prsimrc \
	precharge-d-repeat.prsimrc \
	precharge-e-repeat.prsimrc \
	precharge-f-repeat.prsimrc

include $(top_srcdir)/test/Make.test-tail

