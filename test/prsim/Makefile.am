# "test/prsim/Makefile.am"
#	$Id: Makefile.am,v 1.71 2009/02/28 01:20:47 fang Exp $
# Test auto-Makefile for prsim.  

include $(top_srcdir)/test/Make.test-head

SUBDIRS = . vpi

# .hac inputs
HACKT_PRSIMRC_TEST_SUBJECTS = \
	celem inv oscillator half-celem deep tree interference \
	invs inv-invariant \
	invariant-a invariant-b invariant-c invariant-d \
	invariant-not-X \
	oscillator-wrong \
	interference-a interference-b \
	allow_interference allow_weak_interference \
	inv-alt unused \
	celem-share after-celem-def after-celem after-celem-more \
	pass-gates-def pass-gates excl-rings-def excl-rings \
	bad-pass-gate-a bad-pass-gate-b \
	check-excl-rings-def check-excl-rings \
	check-excl-rings-chan-def check-excl-rings-chan \
	check-excl-fail \
	logic-defs logic-defs-a logic-defs-b \
	and nand or nor array \
	and-nand or-nor nand-or \
	and-nand-alt or-nor-alt nor-and \
	expr-limit-a \
	unstable-a unstable-b \
	weak-unstable-a weak-unstable-b \
	rule_unstable-a rule_unstable-b rule_unstable-c \
	rule_unstable-d rule_unstable-e rule_unstable-f \
	precharge-a precharge-b precharge-c precharge-d \
	precharge-e precharge-f precharge-g precharge-h \
	weak-inv \
	weak-fight-a weak-fight-b \
	weak-vs-weak \
	weak-feedback-a weak-feedback-b \
	weak-yield-a weak-yield-b \
	celem-fb \
	half-weak-a half-weak-b \
	half-keeper-a \
	cross-coupled cross-coupled-weak \
	cross-pulldown \
	cbit \
	cbit_test_stat \
	cross-pull-a \
	bitline-a bitline-b \
	SRAM SRAM-a SRAM-b SRAM-c SRAM-d \
	keeper-a \
	iskeeper-a \
	channel-test-a channel-test-b channel-test-c channel-test-d \
	channel-test-f channel-test-g \
	channel-test-non-QDI-a \
	stuck-bug-a \
	crash-bug-01 \
	after_min_max after_min_max-bad \
	non-CMOS-a non-CMOS-b \
	always_random-a \
	completion_test

# TODO: more cross-coupled test cases
# SRAM cell
# register with read/write ports, cross-coupled

HACKT_PARSE_TEST_PASSES = \
	expr-limit-b \
	large_OR_fanin huge_OR_fanin

HACKT_PRSIMRC_TEST_SCRIPTS_PASSES = \
	comment-01 comment-02 comment-04 comment-06 comment-08 \
	repeat-01 \
	default-mode \
	getlocal-01 getlocal-02 getlocal-03 \
	seed48-01 seed48-02 seed48-03 \
	celem-01 celem-02 celem-03 celem-04 \
	celem-why-01 celem-why-02 \
	inv-00 inv-01 inv-02 inv-03 inv-04 inv-05 inv-07 inv-08 inv-09 \
	inv-11 inv-12 inv-13 inv-14 inv-15 inv-16 inv-17 \
	inv-18 inv-19 inv-20 inv-20a inv-21 inv-21a inv-22 inv-23 \
	inv-24 inv-25 inv-26 inv-27 \
	inv-alt-01 inv-alt-02 inv-alt-03 inv-alt-04 inv-alt-05 inv-alt-06 \
	inv-alt-07 inv-alt-08 inv-alt-09 inv-alt-10 \
	inv-why-01 inv-why-02 \
	invs-01 \
	and-01 nand-01 nand-02 or-01 nor-01 nor-02 nor-04 \
	nor-why-01 nand-why-01 \
	and-nand-why-01 or-nor-why-01 \
	and-nand-alt-why-01 or-nor-alt-why-01 \
	nand-or-why-01 \
	nor-and-why-01 \
	unused-01 \
	ls-01 ls-02 \
	logic-defs-01 logic-defs-a-01 logic-defs-b-01 \
	expr-limit-a-01 \
	large_OR_fanin-01 huge_OR_fanin-01 \
	interference-01 interference-02 interference-03 interference-04 \
	interference-05 interference-06 interference-07 interference-08 \
	interference-09 interference-10 \
	interference-a-01 interference-a-02 \
	interference-b-01 \
	allow_interference-01 allow_interference-02 \
	allow_weak_interference-01 allow_weak_interference-02 \
	paranoid-01 \
	killed-01 \
	rule_unstable-a-01 rule_unstable-b-01 rule_unstable-c-01 \
	rule_unstable-d-01 rule_unstable-e-01 rule_unstable-f-01 \
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
	oscil-why-01 oscil-why-02 \
	oscil-wrong-why-01 \
	celem-rules \
	half-celem-01 \
	celem-share-01 celem-share-02 \
	deep-01 deep-02 \
	tree-01 tree-02 \
	array-01 \
	aliases-08 aliases-09 \
	timing-01 \
	pending-01 pending-02 \
	after-override-01 \
	after-celem-base \
	after-celem-01 after-celem-02 after-celem-03 \
	pass-gates-01 pass-gates-02 pass-gates-03 \
	bad-pass-gate-a-01 bad-pass-gate-b-01 \
	excl-rings-01 excl-rings-02 excl-rings-03 \
	check-excl-rings-02 check-excl-rings-04 check-excl-rings-05 \
	check-excl-rings-chan-02 check-excl-rings-chan-04 \
	check-excl-rings-chan-05 \
	precharge-a-01 precharge-b-01 \
	precharge-c-01 precharge-d-01 \
	precharge-e-01 precharge-f-01 \
	precharge-h-01 \
	weak-inv-01 weak-inv-02 weak-inv-03 \
	weak-fight-a-01 weak-fight-a-02 weak-fight-a-03 weak-fight-a-04 \
	weak-fight-a-05 \
	weak-fight-b-01 \
	weak-vs-weak-01 weak-vs-weak-02 weak-vs-weak-03 \
	weak-vs-weak-04 \
	weak-feedback-a-01 weak-feedback-a-03 \
	weak-feedback-b-01 \
	celem-fb-01 \
	half-keeper-a-01 half-keeper-a-02 \
	cross-coupled-01 cross-coupled-weak-01 \
	cross-coupled-why-01 \
	cbit-01 cbit-02 cbit-03 cbit-04 cbit-05 cbit-06 cbit-07 \
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
	keeper-a-01 \
	iskeeper-a-01 \
	autosave-02 \
	channel-test-a-01 channel-test-a-02 channel-test-a-03 \
	channel-test-a-04 channel-test-a-05 channel-test-a-06 \
	channel-test-a-07 channel-test-a-08 channel-test-a-09 \
	channel-test-a-11 channel-test-a-13 channel-test-a-14 \
	channel-test-a-15 channel-test-a-16 channel-test-a-17 \
	channel-test-a-18 channel-test-a-19 channel-test-a-20 \
	channel-test-a-22 channel-test-a-23 \
	channel-test-a-24 channel-test-a-25 channel-test-a-26 \
	channel-test-a-27 channel-test-a-28 channel-test-a-29 \
	channel-test-a-30 channel-test-a-31 channel-test-a-32 \
	channel-test-a-33 channel-test-a-34 channel-test-a-35 \
	channel-test-a-36 channel-test-a-37 channel-test-a-38 \
	channel-test-a-39 channel-test-a-40 \
	channel-test-a-41 channel-test-a-42 \
	channel-test-b-01 channel-test-b-02 channel-test-b-03 \
	channel-test-b-04 channel-test-b-05 channel-test-b-06 \
	channel-test-b-07 channel-test-b-08 \
	channel-test-b-09 channel-test-b-10 \
	channel-test-b-11 channel-test-b-12 \
	channel-test-b-13 channel-test-b-14 \
	channel-test-b-15 channel-test-b-16 channel-test-b-17 \
	channel-test-b-18 channel-test-b-19 channel-test-b-20 \
	channel-test-b-22 channel-test-b-24 channel-test-b-25 \
	channel-test-b-26 channel-test-b-28 \
	channel-test-b-random-01 channel-test-b-random-02 \
	channel-test-b-random-03 channel-test-b-random-04 \
	channel-test-b-random-05 \
	channel-test-non-QDI-a-01 \
	channel-test-c-01 channel-test-c-03 channel-test-c-04 \
	channel-test-c-05 channel-test-c-06 channel-test-c-07 \
	channel-test-c-08 channel-test-c-09 \
	channel-test-c-13 channel-test-c-14 channel-test-c-15 \
	channel-test-c-16 channel-test-c-17 channel-test-c-18 \
	channel-test-c-19 channel-test-c-30 \
	channel-test-c-33 channel-test-c-34 channel-test-c-35 \
	channel-test-c-36 channel-test-c-37 channel-test-c-38 \
	channel-test-d-01 channel-test-d-03 channel-test-d-04 \
	channel-test-d-06 channel-test-d-07 channel-test-d-08 \
	channel-test-d-13 channel-test-d-14 channel-test-d-19 \
	channel-test-d-30 channel-test-d-33 \
	channel-test-d-34 channel-test-d-35 channel-test-d-36 \
	channel-test-d-37 channel-test-d-38 \
	channel-test-e-01 channel-test-e-02 \
	channel-test-e-05 channel-test-e-06 channel-test-e-07 \
	channel-test-f-01 channel-test-f-02 \
	channel-test-f-05 channel-test-f-06 channel-test-f-07 \
	channel-test-g-01 channel-test-g-02 channel-test-g-03 \
	channel-test-g-04 channel-test-g-05 channel-test-g-06 \
	channel-test-g-07 \
	stuck-bug-a-01 \
	crash-bug-01-a \
	always_random-a-01 \
	reschedule-07 reschedule-08 reschedule-09 reschedule-10 reschedule-11 \
	reschedule-12 reschedule-13 reschedule-14 reschedule-15 \
	reschedule-16 reschedule-17 \
	invariant-a-01 invariant-a-02 invariant-a-03 invariant-a-04 \
	invariant-a-05 invariant-a-06 invariant-a-08 \
	invariant-b-01 invariant-b-02 invariant-b-03 invariant-b-04 \
	invariant-b-05 invariant-b-06 invariant-b-08 \
	invariant-c-01 invariant-c-02 invariant-c-03 \
	invariant-d-01 invariant-d-02 invariant-d-03 \
	inv-invariant-01 inv-invariant-02 inv-invariant-03 inv-invariant-04 \
	invariant-not-X-01 \
	non-CMOS-a-01 non-CMOS-a-02 \
	non-CMOS-b-01 non-CMOS-b-02 \
	assert-fail-01 assert-fail-03 \
	assert-confirm-01 \
	check-excl-fail-01 check-excl-fail-02 check-excl-fail-03 \
	check-excl-fail-04 \
	after_min_max-01 after_min_max-02 after_min_max-03 \
	memstats32-inv memstats64-inv

# subset of prsimrc test that we wish to compare traces against regressions
HACKT_PRSIMTRACE_TEST_SUBJECTS = \
	after-celem-01 after-celem-02 \
	after-override-01 \
	bitline-a-01 bitline-a-02 \
	bitline-b-01 bitline-b-02 \
	channel-test-a-04 channel-test-a-05 channel-test-a-06 \
	channel-test-a-15 channel-test-a-16 \
	channel-test-a-17 channel-test-a-18 \
	channel-test-a-30 channel-test-a-32 \
	channel-test-a-34 channel-test-a-36 \
	channel-test-b-02 channel-test-b-05 \
	channel-test-b-09 channel-test-b-11 \
	channel-test-b-17 channel-test-b-18 \
	channel-test-b-22 channel-test-b-24 \
	channel-test-b-25 channel-test-b-28 \
	channel-test-d-04 channel-test-d-06 \
	channel-test-d-30 channel-test-d-34 \
	check-excl-fail-02 check-excl-fail-03 \
	check-excl-rings-02 check-excl-rings-05 \
	check-excl-rings-chan-02 check-excl-rings-chan-05 \
	cross-coupled-01 cross-coupled-weak-01 \
	excl-rings-02 excl-rings-03 \
	interference-01 interference-02 interference-03 interference-04 \
	interference-05 interference-06 interference-07 interference-08 \
	interference-09 interference-10 \
	interference-a-01 interference-a-02 \
	interference-b-01 \
	inv-00 \
	oscil-unset-01 oscil-unset-03 \
	precharge-c-01 precharge-d-01 precharge-f-01 \
	rule_unstable-a-01 rule_unstable-b-01 rule_unstable-c-01 \
	rule_unstable-d-01 rule_unstable-e-01 rule_unstable-f-01 \
	SRAM-a-01 SRAM-a-02 SRAM-a-03 SRAM-a-04 SRAM-a-05 SRAM-a-06 \
	SRAM-b-01 \
	SRAM-c-01 \
	SRAM-d-01 SRAM-d-02 \
	stuck-bug-a-01 \
	unstable-a-01 unstable-b-01

# prsimtrace test crashes: FIXME!!!
# logic-defs-a-01 logic-defs-b-01

# override defaults for non-matched architectures
if ABI_ILP32
memstats64-inv.prsimtest: memstats64-inv.prsimrc
	$(SKIP_TEST_SCRIPT)
else
memstats32-inv.prsimtest: memstats32-inv.prsimrc
	$(SKIP_TEST_SCRIPT)
endif

# interactive tests:
# sometimes disabled because testing these non-interactively
# doesn't always work as expected on all systems
if INTERACTIVE_TESTS
else
assert-fail-03.prsimrc-out: assert-fail-03.prsimrc
	@$(ECHO) "<<skipped>>" > $@
assert-fail-03.prsimtest: assert-fail-03.prsimrc
	$(SKIP_TEST_SCRIPT)
assert-fail-03.prsimckpttest: assert-fail-03.prsimrc
	$(SKIP_TEST_SCRIPT)
assert-fail-03.prsimckpt: assert-fail-03.prsimrc
	@$(TOUCH) $@
assert-fail-03.prsimckpt-O1: assert-fail-03.prsimrc
	@$(TOUCH) $@
assert-fail-03.prsimckpt-O1-err: assert-fail-03.prsimrc
	@$(TOUCH) $@

check-excl-fail-04.prsimrc-out: check-excl-fail-04.prsimrc
	@$(ECHO) "<<skipped>>" > $@
check-excl-fail-04.prsimtest: check-excl-fail-04.prsimrc
	$(SKIP_TEST_SCRIPT)
check-excl-fail-04.prsimckpttest: check-excl-fail-04.prsimrc
	$(SKIP_TEST_SCRIPT)
check-excl-fail-04.prsimckpt: check-excl-fail-04.prsimrc
	@$(TOUCH) $@
check-excl-fail-04.prsimckpt-O1: check-excl-fail-04.prsimrc
	@$(TOUCH) $@
check-excl-fail-04.prsimckpt-O1-err: check-excl-fail-04.prsimrc
	@$(TOUCH) $@
endif

# non-deterministic tests
if RANDOM_TESTS
else
after_min_max-03.prsimtest: after_min_max-03.prsimrc
	$(SKIP_TEST_SCRIPT)
endif

# positive different test:
# oscil-random-01 and oscil-random-01a are *supposed* to differ (seeds)

# different custom test: just compare two existing output dumps (unfiltered!)
oscil-random-02c.prsimrc-diff: \
		oscil-random-02a.prsimrc-out oscil-random-02b.prsimrc-out
	$(DIFF) oscil-random-02a.prsimrc-out oscil-random-02b.prsimrc-out \
		> $@ ; \
	$(TOUCH) $@

$(srcdir)/cbit_test_stat-01.prsimrc: gen_cbit_test.awk
	{ $(ECHO) "# $@" ; \
	$(ECHO) "# @haco@ cbit_test_stat.haco-a" ; \
	$(AWK) -f $< ;} > $@

channel-test-a-values-X.txt:
	{ $(ECHO) "# $@" ; \
	$(ECHO) "# bunch of 1of2 values" ; \
	$(ECHO) "0" ; \
	$(ECHO) "1" ; $(ECHO) "1" ; \
	$(ECHO) "0" ; $(ECHO) "0" ; $(ECHO) "0" ; \
	$(ECHO) "1" ; $(ECHO) "1" ; $(ECHO) "1" ; $(ECHO) "1" ; \
		} > $@

channel-test-a-values-Y.txt:
	{ $(ECHO) "# $@" ; \
	$(ECHO) "# bunch of 1of4 values" ; \
	$(ECHO) "0" ; $(ECHO) "3" ; $(ECHO) "2" ; $(ECHO) "3" ; \
	$(ECHO) "1" ; $(ECHO) "1" ; $(ECHO) "0" ; $(ECHO) "1" ; \
	$(ECHO) "2" ; $(ECHO) "0" ; $(ECHO) "2" ; $(ECHO) "3" ; \
	$(ECHO) "1" ; $(ECHO) "3" ; $(ECHO) "0" ; $(ECHO) "2" ; \
		} > $@

channel-dont-cares.txt:
	{ $(ECHO) "# $@" ; \
	$(ECHO) "# bunch of X (do not care) values" ; \
	$(ECHO) "X" ; $(ECHO) "X" ; $(ECHO) "X" ; $(ECHO) "X" ; \
		} > $@

# override default
autosave-01.prsimckpt: autosave-01.prsimrcfail-out
	@:
# extra deps
# generated tests needs to be built before auto-dependencies
prsimrc-out.autodepend: $(srcdir)/cbit_test_stat-01.prsimrc

CLEANFILES += cbit_test_stat-01.prsimrc \
	channel-test-a-values-X.txt \
	channel-test-a-values-Y.txt \
	channel-dont-cares.txt
AUTO_IGNORE += cbit_test_stat-01.prsimrc \
	channel-test-a-values-X.txt \
	channel-test-a-values-Y.txt \
	channel-dont-cares.txt
EXTRA_DIST += cbit_test_stat-01.prsimrc gen_cbit_test.awk

# all weak-* tests added (pass AND fail) are consistent with Rajit's prsim
# Fail tests (below) need to be revisited to "do the right thing".

# NOTE: expr-limit-a-01 should fail before limit was increased beyond 255

HACKT_CREATE_TEST_PASSES = \
	large_OR_fanin

HACKT_CREATE_TEST_FAILURES = \
	expr-limit-b

HACKT_CFLAT_TEST_SUBJECTS = \
	large_OR_fanin

HACKT_PRSIMRC_TEST_SCRIPTS_FAILURES = \
	comment-03 comment-05 comment-07 \
	repeat-02 \
	autosave-01 \
	celem-05 \
	inv-06 inv-10 nor-03 \
	inv-why-03 \
	ls-03 ls-04 ls-05 ls-06 \
	cross-pulldown-01 \
	cycle-source-a cycle-source-b \
	aliases-01 aliases-02 aliases-03 aliases-04 \
	aliases-05 aliases-06 aliases-07 \
	check-excl-rings-01 check-excl-rings-03 \
	check-excl-rings-chan-01 check-excl-rings-chan-03 \
	weak-feedback-a-02 weak-feedback-b-02 \
	celem-fb-02 \
	channel-test-a-10 channel-test-a-12 \
	channel-test-a-21 \
	channel-test-b-21 \
	channel-test-b-23 \
	channel-test-b-27 channel-test-b-29 channel-test-b-30 \
	channel-test-c-10 channel-test-c-21 \
	channel-test-d-21 \
	channel-test-e-03 channel-test-e-04 \
	channel-test-f-03 channel-test-f-04 \
	channel-test-non-QDI-a-02 \
	precharge-g-01 \
	invariant-a-07 invariant-b-07 \
	assert-fail-02 \
	check-excl-fail-05 \
	after_min_max-bad-01 \
	reschedule-01 reschedule-02 reschedule-03 reschedule-04 reschedule-05 \
	reschedule-06 reschedule-18 \
	oscil-trace-01 oscil-trace-02

# KNOWN BUG (fails, but should not): cross-pulldown-01

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

# KNOWN BUG: failing getopt because is not re-entrant
# revisit this after re-implementing getopt...
srcdir_EXTRA_DIST += compile-driver-a.hac compile-driver-a-01.prsimrc

include $(top_srcdir)/test/Make.test-tail

