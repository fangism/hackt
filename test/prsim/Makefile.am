# "test/prsim/Makefile.am"
#	$Id: Makefile.am,v 1.137 2011/06/03 23:19:35 fang Exp $
# Test auto-Makefile for prsim.  

include $(top_srcdir)/test/Make.test-head

SUBDIRS = . vpi

# .hac inputs
HACKT_PRSIMRC_TEST_SUBJECTS = \
	celem inv oscillator \
	half-celem half-celem-b \
	deep deep2 deep3 \
	tree interference \
	invs inv-invariant \
	invariant-a invariant-b invariant-c invariant-d invariant-e \
	invariant-f \
	invariant-not-X \
	evaluate-test-a \
	oscillator-bounded \
	oscillator-wrong \
	interference-a interference-b \
	allow_interference allow_weak_interference \
	inv-alt unused \
	celem-share after-celem-def after-celem after-celem-more \
	celem-keeper \
	pass-gates-def pass-gates excl-rings-def excl-rings \
	bad-pass-gate-a bad-pass-gate-b \
	check-excl-rings-def check-excl-rings \
	check-excl-rings-chan-def check-excl-rings-chan \
	check-excl-fail \
	check-excl-rings-priv \
	logic-defs logic-defs-a logic-defs-b logic-defs-c \
	and nand or nor \
	array array-b \
	and-nand or-nor nand-or \
	and-nand-alt or-nor-alt nor-and \
	expr-limit-a \
	timing-precedence-a \
	unstable-a unstable-b \
	weak-unstable-a weak-unstable-b \
	attrib-unstab-a \
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
	cbit_test_stat_alt \
	cross-pull-a \
	bitline-a bitline-b bitline-c \
	SRAM SRAM-a SRAM-b SRAM-c SRAM-d \
	keeper-a \
	iskeeper-a \
	isckeeper-a \
	bad-isckeeper-a bad-isckeeper-b bad-isckeeper-c bad-isckeeper-d \
	channel-test-a channel-test-b channel-test-c channel-test-d \
	channel-test-f channel-test-g channel-test-h \
	channel-test-non-QDI-a \
	channel-test-i channel-test-j channel-test-k channel-test-l \
	channel-ledr-a \
	channel-ledr-b \
	channel-ledr-c \
	channel-array-a channel-array-b channel-array-c \
	channel-bd4p-test-a \
	channel-bd2p-test-a \
	channel-bd2p-test-b \
	channel-clocked-test-a \
	clock-source-test-a \
	eledr-buffer eledr-buffer-a eledr-buffer-b \
	type-test-a type-test-b type-test-c type-test-d \
	type-test-e type-test-f type-test-g \
	ACX-PR-6347 \
	ACX-PR-6376 ACX-PR-6376b \
	ACX-PR-6386 ACX-PR-6386b \
	ACX-PR-6393 \
	ACX-PR-6629 \
	ACX-PR-6718 ACX-PR-6718b ACX-PR-6718c ACX-PR-6718d \
	stuck-bug-a \
	crash-bug-01 \
	after_min_max after_min_max-bad \
	non-CMOS-a non-CMOS-b \
	always_random-a \
	relaxed-a relaxed-b \
	completion_test \
	set-multi-a set-multi-b \
	getall-a getall-a-def getall-b getall-c \
	sreset-preset-a sreset-preset-b sreset-preset-c \
	sreset-preset-d sreset-preset-e sreset-preset-f \
	sreset-preset-g sreset-preset-h sreset-preset-i \
	preset-delay-a \
	xor-test-a xor-test-b \
	pchb pcehb \
	pchb-bad-a pcehb-bad-a \
	pchb-test-a pcfb-test-a \
	pcehb-test-a pcefb-test-a \
	pchb-test-b pcfb-test-b \
	pcehb-test-b pcefb-test-b \
	power-supplies-a power-supplies-b power-supplies-c power-supplies-d \
	power-supplies-e power-supplies-f \
	pseudo-flop-a pseudo-flop-b \
	pseudo-flop-c pseudo-flop-d \
	flop-timing flop-timing-a flop-timing-b \
	atomic-a atomic-b atomic-c atomic-d atomic-e atomic-f atomic-g \
	atomic-h atomic-i atomic-i2 atomic-j

# TODO: more cross-coupled test cases
# SRAM cell
# register with read/write ports, cross-coupled

# after getall works on arrays:
#	array-02

HACKT_PARSE_TEST_PASSES = \
	expr-limit-b \
	large_OR_fanin huge_OR_fanin

HACKT_PRSIMRC_TEST_SCRIPTS_PASSES = \
	comment-01 comment-02 comment-04 comment-06 comment-08 \
	precision-01 \
	time-format-01 time-format-02 time-format-03 time-format-04 \
	repeat-01 \
	default-mode \
	default-after-01 default-after-02 default-after-03 default-after-04 \
	relaxed-a-01 \
	relaxed-b-01 \
	getlocal-01 getlocal-02 getlocal-03 \
	getall-a-01 getall-a-02 \
	getall-b-01 \
	getall-c-01 \
	getports-01 \
	getcommonports-01 \
	seed48-01 seed48-02 seed48-03 \
	celem-01 celem-02 celem-03 celem-04 \
	celem-why-01 celem-why-02 \
	inv-00 inv-01 inv-02 inv-03 inv-04 inv-05 inv-07 inv-08 inv-09 \
	inv-11 inv-12 inv-13 inv-14 inv-15 inv-16 inv-17 \
	inv-18 inv-19 inv-20 inv-20a inv-21 inv-21a inv-22 inv-23 \
	inv-24 inv-25 inv-26 inv-27 inv-29 inv-30 \
	inv-37 inv-38 \
	inv-alt-01 inv-alt-02 inv-alt-03 inv-alt-04 inv-alt-05 inv-alt-06 \
	inv-alt-07 inv-alt-08 inv-alt-09 inv-alt-10 \
	inv-why-01 inv-why-02 \
	invs-01 \
	and-01 nand-01 nand-02 nand-03 nand-03b \
	or-01 nor-01 nor-02 nor-02b nor-03 nor-04 \
	nor-why-01 nand-why-01 \
	and-nand-why-01 or-nor-why-01 \
	and-nand-alt-why-01 or-nor-alt-why-01 \
	nand-or-why-01 \
	nor-and-why-01 \
	tcount-01 \
	unused-01 unused-02 unused-03 unused-04 unused-05 unused-07 \
	ls-01 ls-02 \
	type-test-a-01 type-test-b-01 type-test-c-01 type-test-d-01 \
	type-test-d-02 type-test-e-01 type-test-e-02 type-test-f-01 \
	type-test-g-01 \
	ACX-PR-6347 \
	ACX-PR-6376 ACX-PR-6376b \
	ACX-PR-6386 ACX-PR-6386b \
	ACX-PR-6393 \
	ACX-PR-6628 \
	ACX-PR-6718 ACX-PR-6718b ACX-PR-6718c ACX-PR-6718d \
	logic-defs-01 logic-defs-a-01 logic-defs-b-01 logic-defs-c-01 \
	expr-limit-a-01 \
	large_OR_fanin-01 huge_OR_fanin-01 \
	interference-01 interference-02 interference-03 interference-04 \
	interference-05 interference-06 interference-07 interference-08 \
	interference-07b interference-08b \
	interference-07c interference-08c \
	interference-09 interference-10 interference-12 \
	interference-a-01 interference-a-02 \
	interference-b-01 \
	allow_interference-01 allow_interference-02 \
	allow_weak_interference-01 allow_weak_interference-02 \
	driven-01 driven-04 driven-05 driven-08 \
	paranoid-01 \
	killed-01 \
	rule_unstable-a-01 rule_unstable-b-01 rule_unstable-c-01 \
	rule_unstable-b-01b \
	rule_unstable-d-01 rule_unstable-e-01 rule_unstable-f-01 \
	attrib-unstab-a-01 attrib-unstab-a-02 \
	unstable-01 unstable-02 unstable-03 \
	unstable-a-01 unstable-b-01 \
	weak-unstable-a-01 weak-unstable-a-02 weak-unstable-a-03 \
	weak-unstable-a-04 \
	weak-unstable-b-01 weak-unstable-b-02 weak-unstable-b-03 \
	weak-unstable-b-04 \
	weak-yield-a-01 weak-yield-b-01 \
	breakpt-01 \
	oscil-break-01 oscil-break-02 oscil-break-03 \
	oscil-random-01 oscil-random-01a \
	oscil-random-02 oscil-random-02a oscil-random-02b oscil-random-02c \
	oscil-random-03 oscil-random-04 oscil-random-05 \
	oscil-random-06 oscil-random-07 \
	oscil-what-01 oscil-what-02 \
	oscil-unset-01 oscil-unset-02 oscil-unset-03 \
	oscil-backtrace-01 oscil-backtrace-02 \
	oscil-status-01 \
	oscil-why-01 oscil-why-02 \
	oscil-wrong-why-01 \
	oscil-freeze-01 \
	oscil-upset-01 oscil-upset-02 \
	celem-rules \
	half-celem-01 \
	half-celem-b-01 \
	celem-share-01 celem-share-02 \
	deep-01 deep-02 \
	tree-01 tree-02 \
	array-01 array-02 \
	array-b-01 \
	aliases-08 aliases-09 \
	timing-01 \
	timing-precedence-a-01 \
	timing-bounded-01 timing-bounded-02 \
	pending-01 pending-02 \
	after-override-01 \
	after-celem-base \
	after-celem-01 after-celem-02 after-celem-03 \
	pass-gates-01 pass-gates-02 pass-gates-03 \
	bad-pass-gate-a-01 bad-pass-gate-b-01 \
	excl-rings-01 excl-rings-02 excl-rings-03 excl-rings-04 \
	excl-rings-02b excl-rings-03b excl-rings-03c excl-rings-03d \
	check-excl-rings-02 check-excl-rings-04 check-excl-rings-05 \
	check-excl-rings-06 \
	check-excl-rings-chan-02 check-excl-rings-chan-04 \
	check-excl-rings-chan-05 \
	precharge-a-01 precharge-b-01 \
	precharge-c-01 precharge-d-01 \
	precharge-e-01 precharge-f-01 \
	precharge-g-01 \
	precharge-h-01 \
	weak-inv-01 weak-inv-02 weak-inv-03 \
	weak-fight-a-01 weak-fight-a-02 weak-fight-a-03 weak-fight-a-04 \
	weak-fight-a-05 weak-fight-a-06 weak-fight-a-07 \
	weak-fight-b-01 \
	weak-fight-b-02 weak-fight-b-03 weak-fight-b-04 weak-fight-b-05 \
	weak-vs-weak-01 weak-vs-weak-02 weak-vs-weak-03 \
	weak-vs-weak-04 \
	weak-feedback-a-01 weak-feedback-a-03 weak-feedback-a-04 \
	weak-feedback-b-01 \
	celem-fb-01 celem-fb-03 \
	half-keeper-a-01 half-keeper-a-02 \
	cross-coupled-01 cross-coupled-weak-01 \
	cross-coupled-why-01 \
	cbit-01 cbit-02 cbit-02b cbit-02c cbit-02d cbit-02e \
	cbit-03 cbit-04 cbit-05 cbit-05b cbit-06 \
	cbit-07 cbit-08 cbit-09 cbit-09b cbit-09c cbit-10 cbit-11 \
	cbit_test_stat-01 \
	cbit_test_stat_alt-01 \
	cross-pull-a-01 cross-pull-a-02 \
	cross-pull-a-01b cross-pull-a-02b \
	bitline-a-01 bitline-a-02 \
	bitline-b-01 bitline-b-02 \
	bitline-c-01 bitline-c-02 \
	half-weak-a-01 half-weak-a-01a half-weak-a-02 half-weak-a-02a \
	half-weak-a-03 half-weak-a-03a half-weak-a-04 half-weak-a-04a \
	half-weak-b-01 half-weak-b-01a half-weak-b-02 half-weak-b-02a \
	half-weak-b-03 half-weak-b-03a half-weak-b-04 half-weak-b-04a \
	SRAM-a-01 SRAM-a-02 SRAM-a-03 SRAM-a-04 SRAM-a-05 SRAM-a-06 \
	SRAM-b-01 SRAM-b-02 SRAM-b-03 \
	SRAM-c-01 SRAM-c-02 SRAM-c-03 \
	SRAM-d-01 SRAM-d-02 \
	keeper-a-01 keeper-a-01b \
	iskeeper-a-01 iskeeper-a-02 \
	isckeeper-a-01 isckeeper-a-02 \
	celem-keeper-01 celem-keeper-02 \
	celem-keeper-03 celem-keeper-03b celem-keeper-03c celem-keeper-03d \
	autosave-02 \
	channel-test-a-01 channel-test-a-02 channel-test-a-03 \
	channel-test-a-04 channel-test-a-05 channel-test-a-06 \
	channel-test-a-07 channel-test-a-08 channel-test-a-09 \
	channel-test-a-11 channel-test-a-13 channel-test-a-14 \
	channel-test-a-15 channel-test-a-16 channel-test-a-17 \
	channel-test-a-18 channel-test-a-19 channel-test-a-20 \
	channel-test-a-22 channel-test-a-23 \
	channel-test-a-24 channel-test-a-24b channel-test-a-24c \
	channel-test-a-25 channel-test-a-26 \
	channel-test-a-27 channel-test-a-28 channel-test-a-29 \
	channel-test-a-30 channel-test-a-31 channel-test-a-32 \
	channel-test-a-33 channel-test-a-34 channel-test-a-35 \
	channel-test-a-36 channel-test-a-37 channel-test-a-38 \
	channel-test-a-39 channel-test-a-40 \
	channel-test-a-41 channel-test-a-42 \
	channel-test-a-43 channel-test-a-44 channel-test-a-45 \
	channel-test-a-46 channel-test-a-47 \
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
	channel-test-b-31 channel-test-b-32 \
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
	channel-test-h-01 channel-test-h-02 channel-test-h-03 \
	channel-test-h-04 channel-test-h-05 channel-test-h-06 \
	channel-test-h-08 channel-test-h-09 \
	channel-test-i-01 channel-test-i-02 \
	channel-test-j-01 channel-test-j-02 \
	channel-test-k-01 channel-test-k-02 \
	channel-ledr-a-01 channel-ledr-a-02 channel-ledr-a-03 \
	channel-ledr-a-04 channel-ledr-a-05 \
	channel-ledr-a-07 channel-ledr-a-08 channel-ledr-a-09 \
	channel-ledr-a-10 \
	channel-eledr-a-01 channel-eledr-a-02 channel-eledr-a-03 \
	channel-eledr-a-04 channel-eledr-a-05 \
	channel-eledr-a-07 channel-eledr-a-08 \
	channel-eledr-c-01 channel-eledr-c-02 channel-eledr-c-03 \
	channel-eledr-c-04 channel-eledr-c-05 \
	channel-eledr-c-07 channel-eledr-c-08 \
	channel-ledr-b-01 \
	channel-ledr-c-01 channel-ledr-c-02 channel-ledr-c-03 \
	channel-ledr-c-05 \
	channel-ledr-d-01 channel-ledr-d-02 \
	channel-eledr-d-02 \
	channel-status-a-01 channel-status-a-02 channel-status-a-03 \
	channel-status-c-01 \
	channel-status-f-01 \
	channel-status-g-01 \
	channel-status-h-01 \
	channel-assert-a-01 channel-assert-a-10 \
	channel-assert-c-01 channel-assert-c-08 \
	channel-assert-d-01 \
	channel-assert-f-01 \
	channel-assert-g-01 \
	channel-assert-h-01 \
	channel-array-a-03 channel-array-a-04 channel-array-a-05 \
	channel-array-b-01 \
	channel-array-c-01 \
	channel-bd4p-test-a-01 \
	channel-bd4p-test-a-ev-03 channel-bd4p-test-a-av-03 \
	channel-bd4p-test-a-en-03 channel-bd4p-test-a-an-03 \
	channel-bd4p-test-a-ev-04 channel-bd4p-test-a-av-04 \
	channel-bd4p-test-a-en-04 channel-bd4p-test-a-an-04 \
	channel-bd4p-test-a-ev-08 \
	channel-bd4p-test-a-ev-09 channel-bd4p-test-a-ev-09b \
	channel-bd4p-test-a-ev-19 channel-bd4p-test-a-ev-19b \
	channel-bd4p-test-a-20 channel-bd4p-test-a-22 channel-bd4p-test-a-23 \
	channel-bd4p-test-a-24 channel-bd4p-test-a-25 channel-bd4p-test-a-26 \
	channel-bd4p-test-a-27 channel-bd4p-test-a-28 channel-bd4p-test-a-29 \
	channel-bd4p-test-a-31 channel-bd4p-test-a-33 \
	channel-bd4p-test-a-ev-34 \
	channel-bd4p-test-a-ev-40 channel-bd4p-test-a-ev-41 \
	channel-bd4p-test-a-ev-43 channel-bd4p-test-a-ev-44 \
	channel-bd4p-test-a-ev-45 \
	channel-bd4p-test-a-46 \
	channel-bd4p-test-a-ev-48 \
	channel-bd4p-test-g-01 \
	channel-bd4p-test-g-ev-03 channel-bd4p-test-g-ev-04 \
	channel-bd4p-test-g-ev-08 \
	channel-bd4p-test-g-ev-09 channel-bd4p-test-g-ev-09b \
	channel-bd2p-test-a-01 \
	channel-bd2p-test-a-A-03 channel-bd2p-test-a-A-04 \
	channel-bd2p-test-a-A-08 \
	channel-bd2p-test-a-A-09 channel-bd2p-test-a-A-09b \
	channel-bd2p-test-a-B-03 channel-bd2p-test-a-B-04 \
	channel-bd2p-test-a-B-08 \
	channel-bd2p-test-a-B-09 channel-bd2p-test-a-B-09b \
	channel-bd2p-test-a-C-03 channel-bd2p-test-a-C-04 \
	channel-bd2p-test-a-C-08 \
	channel-bd2p-test-a-C-09 channel-bd2p-test-a-C-09b \
	channel-bd2p-test-a-D-03 channel-bd2p-test-a-D-04 \
	channel-bd2p-test-a-D-08 \
	channel-bd2p-test-a-D-09 channel-bd2p-test-a-D-09b \
	channel-bd2p-test-a-20 \
	channel-bd2p-test-a-22 channel-bd2p-test-a-23 channel-bd2p-test-a-24 \
	channel-bd2p-test-a-25 channel-bd2p-test-a-26 channel-bd2p-test-a-27 \
	channel-bd2p-test-a-28 channel-bd2p-test-a-29 \
	channel-bd2p-test-a-31 channel-bd2p-test-a-33 \
	channel-bd2p-test-a-A-34 \
	channel-bd2p-test-a-A-40 channel-bd2p-test-a-A-41 \
	channel-bd2p-test-a-A-43 channel-bd2p-test-a-A-44 \
	channel-bd2p-test-a-A-45 \
	channel-bd2p-test-a-46 \
	channel-bd2p-test-a-A-48 \
	channel-bd2p-test-b-01 \
	channel-bd2p-test-b-A-03 channel-bd2p-test-b-A-04 \
	channel-bd2p-test-b-A-08 \
	channel-bd2p-test-b-A-09 channel-bd2p-test-b-A-09b \
	channel-clocked-test-a-01 \
	channel-clocked-test-a-02 channel-clocked-test-a-02b \
	channel-clocked-test-a-03 \
	channel-clocked-test-a-04 channel-clocked-test-a-04b \
	channel-clocked-test-a-05 channel-clocked-test-a-05b \
	channel-clocked-test-a-05c \
	channel-clocked-test-a-07b \
	channel-clocked-test-a-07d channel-clocked-test-a-07e \
	channel-clocked-test-a-08 channel-clocked-test-a-08b \
	channel-clocked-test-a-09 channel-clocked-test-a-10 \
	channel-clocked-test-a-11 \
	channel-clocked-test-b-02 channel-clocked-test-b-02b \
	channel-clocked-test-b-03 \
	channel-clocked-test-b-04 channel-clocked-test-b-04b \
	channel-clocked-test-b-05 \
	channel-clocked-test-b-08 \
	clock-source-test-a-01 clock-source-test-a-02 clock-source-test-a-03 \
	clock-source-test-a-04 \
	eledr-buffer-a-01 eledr-buffer-a-02 eledr-buffer-a-03 \
	eledr-buffer-a-04 eledr-buffer-a-05 eledr-buffer-a-06 \
	eledr-buffer-a-07 \
	eledr-buffer-b-01 eledr-buffer-b-02 eledr-buffer-b-03 \
	eledr-buffer-b-04 eledr-buffer-b-05 eledr-buffer-b-06 \
	eledr-buffer-b-07 \
	channel-ledr-assert-a-01 \
	channel-eledr-assert-a-01 \
	ACX-PR-6629b \
	stuck-bug-a-01 \
	crash-bug-01-a \
	always_random-a-01 \
	reschedule-07 reschedule-08 reschedule-09 reschedule-10 reschedule-11 \
	reschedule-12 reschedule-13 reschedule-14 reschedule-15 \
	reschedule-16 reschedule-17 \
	set-now-01 set-now-02 \
	invariant-a-01 invariant-a-02 invariant-a-03 invariant-a-04 \
	invariant-a-05 invariant-a-06 invariant-a-08 \
	invariant-b-01 invariant-b-02 invariant-b-03 invariant-b-04 \
	invariant-b-05 invariant-b-06 invariant-b-08 \
	invariant-c-01 invariant-c-02 invariant-c-03 \
	invariant-d-01 invariant-d-02 invariant-d-03 \
	invariant-e-01 \
	invariant-f-01 invariant-f-02 invariant-f-03 \
	inv-invariant-01 inv-invariant-02 inv-invariant-03 inv-invariant-04 \
	invariant-not-X-01 \
	evaluate-test-a-01 \
	non-CMOS-a-01 non-CMOS-a-02 \
	non-CMOS-b-01 non-CMOS-b-02 \
	assert-fail-01 assert-fail-03 \
	assert-confirm-01 \
	set-multi-a-01 \
	set-multi-b-01 set-multi-b-02 \
	check-excl-fail-01 check-excl-fail-02 check-excl-fail-03 \
	check-excl-fail-04 \
	after_min_max-01 after_min_max-02 after_min_max-03 \
	dirs-02 dirs-03 dirs-04 dirs-05 dirs-06 dirs-07 \
	memstats32-inv memstats64-inv \
	history-01 history-02 history-03 history-04 \
	separator-01 separator-02 separator-03 separator-04 \
	separator-05 separator-06 \
	dequeue-01 dequeue-02 \
	freeze-01 freeze-03 freeze-04 freeze-05 freeze-06 \
	upset-01 \
	sreset-preset-a-01 sreset-preset-a-04 \
	sreset-preset-b-01 \
	sreset-preset-c-01 sreset-preset-c-02 sreset-preset-c-03 \
	sreset-preset-c-04 \
	sreset-preset-d-01 \
	preset-delay-a-01 \
	xor-test-a-01 xor-test-a-02 \
	xor-test-b-01 \
	pchb-test-a-01 pchb-test-a-02 \
	pcfb-test-a-01 pcfb-test-a-02 \
	pcehb-test-a-01 pcehb-test-a-02 \
	pcefb-test-a-01 pcefb-test-a-02 \
	pchb-test-b-01 \
	pcfb-test-b-01 \
	pcehb-test-b-01 \
	pcefb-test-b-01 \
	power-supplies-a-01 power-supplies-a-02 power-supplies-a-03 \
	power-supplies-b-01 power-supplies-b-02 \
	power-supplies-c-01 power-supplies-d-01 \
	power-supplies-e-01 power-supplies-f-01 \
	x-all-01 x-all-02 x-all-03 x-all-04 x-all-05 \
	pseudo-flop-a-01 \
	pseudo-flop-b-01 \
	pseudo-flop-c-01 \
	pseudo-flop-d-01 \
	flop-timing-a-01 flop-timing-a-02 \
	flop-timing-b-01 flop-timing-b-02 \
	atomic-a-03 atomic-a-05 \
	atomic-b-01 \
	atomic-c-01 atomic-c-02 \
	atomic-d-01 \
	atomic-e-01 atomic-e-02 \
	atomic-f-01 \
	type-scope-01 type-scope-04

# subset of prsimrc test that we wish to compare traces against regressions
HACKT_PRSIMTRACE_TEST_SUBJECTS = \
	after-celem-01 after-celem-02 \
	after-override-01 \
	bitline-a-01 bitline-a-02 \
	bitline-b-01 bitline-b-02 \
	bitline-c-01 bitline-c-02 \
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
	channel-ledr-a-03 channel-ledr-a-05 \
	channel-ledr-a-07 channel-ledr-a-08 \
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
	unstable-a-01 unstable-b-01 \
	x-all-01 \
	atomic-b-01 \
	atomic-d-01 \
	atomic-f-01

# prsimtrace test crashes: FIXME!!!
# logic-defs-a-01 logic-defs-b-01

# these tests should be subset of PRSIMRC_TEST_PASSES
HACKT_PRSIMVCD_TEST_SUBJECTS = \
	inv-01 after-celem-01 \
	bitline-a-01 bitline-a-02 \
	channel-test-a-04 channel-test-a-09 \
	oscil-unset-01 \
	unstable-a-01 \
	x-all-01

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
timing-bounded-01.prsimtest: timing-bounded-01.prsimrc
	$(SKIP_TEST_SCRIPT)
timing-bounded-02.prsimtest: timing-bounded-02.prsimrc
	$(SKIP_TEST_SCRIPT)
after_min_max-03.prsimtest: after_min_max-03.prsimrc
	$(SKIP_TEST_SCRIPT)
channel-test-a-47.prsimtest: channel-test-a-47.prsimrc
	$(SKIP_TEST_SCRIPT)
# has random source:
channel-test-b-11.prsimtracetest: channel-test-b-11.prsimrc
	$(SKIP_TEST_SCRIPT)
# eval-order random
precharge-c-01.prsimtracetest: precharge-c-01.prsimrc
	$(SKIP_TEST_SCRIPT)
precharge-d-01.prsimtracetest: precharge-d-01.prsimrc
	$(SKIP_TEST_SCRIPT)
precharge-f-01.prsimtracetest: precharge-f-01.prsimrc
	$(SKIP_TEST_SCRIPT)
default-after-02.prsimtest: default-after-02.prsimrc
	$(SKIP_TEST_SCRIPT)
default-after-03.prsimtest: default-after-03.prsimrc
	$(SKIP_TEST_SCRIPT)
default-after-04.prsimtest: default-after-04.prsimrc
	$(SKIP_TEST_SCRIPT)
eledr-buffer-b-03.prsimtest: eledr-buffer-b-03.prsimrc
	$(SKIP_TEST_SCRIPT)
eledr-buffer-b-04.prsimtest: eledr-buffer-b-04.prsimrc
	$(SKIP_TEST_SCRIPT)
eledr-buffer-b-06.prsimtest: eledr-buffer-b-06.prsimrc
	$(SKIP_TEST_SCRIPT)
eledr-buffer-b-07.prsimtest: eledr-buffer-b-07.prsimrc
	$(SKIP_TEST_SCRIPT)
# set random values:
inv-37.prsimtest: inv-37.prsimrc
	$(SKIP_TEST_SCRIPT)
cbit-08.prsimtest: cbit-08.prsimrc
	$(SKIP_TEST_SCRIPT)
endif

# positive different test:
# oscil-random-01 and oscil-random-01a are *supposed* to differ (seeds)

# different custom test: just compare two existing output dumps (unfiltered!)
oscil-random-02c.prsimrc-diff: \
		oscil-random-02a.prsimrc-out-filter oscil-random-02b.prsimrc-out-filter
	$(DIFF) -u oscil-random-02a.prsimrc-out-filter oscil-random-02b.prsimrc-out-filter \
		> $@ ; \
	$(TOUCH) $@

$(srcdir)/cbit_test_stat-01.prsimrc: gen_cbit_test.awk
	{ $(ECHO) "# $@" ; \
	$(ECHO) "# @haco@ cbit_test_stat.haco-c" ; \
	$(AWK) -f $< ;} > $@

cbit_test_stat_alt-01.prsimrc-out: $(srcdir)/cbit_test_stat-01.prsimrc

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

# failing test that produces needed checkpoint
ACX-PR-6629.prsimckpt: ACX-PR-6629.prsimrcfail-out
	@:

# extra deps
# generated tests needs to be built before auto-dependencies
prsimrc-out.autodepend: $(srcdir)/cbit_test_stat-01.prsimrc

comment-ckpt-01.prsimrcfail-out: comment-01.prsimckpt

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
	getall-a getall-b getall-c \
	type-test-d type-test-e type-test-f type-test-g \
	ACX-PR-6347 \
	ACX-PR-6386 \
	ACX-PR-6393 \
	large_OR_fanin

HACKT_CREATE_TEST_FAILURES = \
	expr-limit-b

HACKT_ALLOC_TEST_PASSES = \
	getall-a getall-b getall-c \
	ACX-PR-6347 \
	ACX-PR-6386 \
	type-test-d type-test-e type-test-f type-test-g

HACKT_CFLAT_TEST_SUBJECTS = \
	large_OR_fanin

HACKT_PRSIMRC_TEST_SCRIPTS_FAILURES = \
	comment-03 comment-05 comment-07 \
	repeat-02 \
	bad-set-01 \
	autosave-01 \
	celem-05 \
	inv-06 inv-10 inv-28 \
	inv-31 inv-32 inv-33 inv-34 inv-35 inv-36 \
	inv-why-03 \
	unused-06 \
	ls-03 ls-04 ls-05 ls-06 \
	dirs-08 \
	array-07 array-08 array-09 \
	cross-pulldown-01 \
	cycle-source-a cycle-source-b \
	aliases-01 aliases-02 aliases-03 aliases-04 \
	aliases-05 aliases-06 aliases-07 \
	check-excl-rings-01 check-excl-rings-03 \
	check-excl-rings-chan-01 check-excl-rings-chan-03 \
	check-excl-rings-priv-01 check-excl-rings-priv-02 \
	weak-feedback-a-02 weak-feedback-b-02 \
	celem-fb-02 \
	channel-test-a-10 channel-test-a-10b channel-test-a-12 \
	channel-test-a-21 \
	channel-test-b-21 \
	channel-test-b-23 \
	channel-test-b-27 channel-test-b-29 channel-test-b-30 \
	channel-test-c-10 channel-test-c-21 \
	channel-test-d-21 \
	channel-test-e-03 channel-test-e-04 \
	channel-test-f-03 channel-test-f-04 \
	channel-test-h-07 \
	channel-test-k-03 \
	channel-test-l-01 \
	channel-test-non-QDI-a-02 \
	channel-assert-a-02 channel-assert-a-03 channel-assert-a-04 \
	channel-assert-a-05 channel-assert-a-06 channel-assert-a-07 \
	channel-assert-a-08 channel-assert-a-09 \
	channel-assert-a-11 channel-assert-a-12 \
	channel-assert-a-13 channel-assert-a-14 \
	channel-assert-c-02 channel-assert-c-03 channel-assert-c-04 \
	channel-assert-c-05 channel-assert-c-06 channel-assert-c-07 \
	channel-ledr-assert-a-02 \
	channel-eledr-assert-a-02 \
	channel-ledr-a-06 \
	channel-ledr-c-06 \
	channel-eledr-a-06 \
	channel-eledr-c-06 \
	channel-array-a-01 channel-array-a-02 \
	channel-array-b-02 \
	channel-bd4p-test-a-ev-10 channel-bd4p-test-a-ev-10b \
	channel-bd4p-test-a-21 \
	channel-bd4p-test-a-ev-39 \
	channel-bd2p-test-a-21 \
	ACX-PR-6629 \
	interference-11 \
	interference-13 \
	interference-14 interference-15 \
	driven-02 driven-03 \
	driven-06 driven-07 driven-09 driven-10 \
	unstable-04 unstable-05 unstable-06 \
	invariant-a-07 invariant-a-09 \
	invariant-b-07 \
	channel-unstable-01 channel-unstable-02 \
	assert-fail-02 \
	check-excl-fail-05 \
	after_min_max-bad-01 \
	reschedule-01 reschedule-02 reschedule-03 reschedule-04 reschedule-05 \
	reschedule-06 reschedule-18 \
	oscil-trace-01 oscil-trace-02 \
	dirs-01 \
	freeze-02 \
	comment-ckpt-01 \
	sreset-preset-a-02 sreset-preset-a-03 \
	sreset-preset-b-02 \
	sreset-preset-e-01 sreset-preset-f-01 \
	sreset-preset-g-01 sreset-preset-h-01 sreset-preset-i-01 \
	bad-isckeeper-a-01 bad-isckeeper-a-02 \
	bad-isckeeper-b-01 bad-isckeeper-c-01 bad-isckeeper-d-01 \
	xor-test-b-02 \
	pcfb-test-b-02 \
	pchb-test-b-02 \
	pcefb-test-b-02 \
	pcehb-test-b-02 \
	channel-clocked-test-a-06 channel-clocked-test-a-06b \
	channel-clocked-test-a-07 channel-clocked-test-a-07c \
	channel-clocked-test-b-06 \
	channel-clocked-test-b-07 \
	reset-test-01 \
	atomic-a-01 atomic-a-02 atomic-a-04 \
	atomic-g-01 atomic-h-01 \
	atomic-i-01 atomic-i2-01 atomic-j-01 \
	type-scope-02 type-scope-03 type-scope-05

# KNOWN BUG (fails, but should not): cross-pulldown-01

HACKT_PRSIM_CKPT_TEST_SCRIPTS_BASES = $(HACKT_PRSIMRC_TEST_SCRIPTS_PASSES)

HACKT_PRSIM_OPT_TEST_SCRIPTS_BASES = $(HACKT_PRSIM_CKPT_TEST_SCRIPTS_BASES)

HACKNET_TEST_SCRIPTS_PASSES = \
	pchb-test-a pcfb-test-a \
	pcehb-test-a pcefb-test-a \
	pchb-test-b pcfb-test-b \
	pcehb-test-b pcefb-test-b

srcdir_EXTRA_DIST += \
	half-keeper-cycle.prsimrc \
	precharge-c-repeat.prsimrc \
	precharge-d-repeat.prsimrc \
	precharge-e-repeat.prsimrc \
	precharge-f-repeat.prsimrc \
	history-echo.prsimrc

# KNOWN BUG: failing getopt because is not re-entrant
# revisit this after re-implementing getopt...
srcdir_EXTRA_DIST += compile-driver-a.hac compile-driver-a-01.prsimrc

include $(top_srcdir)/test/Make.test-tail

