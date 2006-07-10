# "test/prsim/Makefile.am"
#	$Id: Makefile.am,v 1.9.2.1 2006/07/10 02:28:19 fang Exp $
# Test auto-Makefile for prsim.  

include $(top_srcdir)/test/Make.test-head

# .hac inputs
HACKT_PRSIMRC_TEST_SUBJECTS = celem inv oscillator half-celem deep tree \
	celem-share after-celem-def after-celem after-celem-more \
	pass-gates-def pass-gates excl-rings-def excl-rings \
	check-excl-rings-def check-excl-rings \
	logic-defs and nand or nor

HACKT_PRSIMRC_TEST_SCRIPTS_BASES = \
	celem-01 celem-02 celem-03 celem-04 \
	inv-00 inv-01 inv-02 inv-03 inv-04 inv-05 inv-06 inv-07 inv-08 inv-09 \
	inv-10 inv-11 inv-12 \
	and-01 nand-01 or-01 nor-01 \
	cycle-source-a cycle-source-b \
	unstable-01 unstable-02 unstable-03 \
	oscil-break-01 oscil-break-02 oscil-break-03 \
	oscil-random-01 oscil-what-01 \
	half-celem-01 \
	celem-share-01 \
	deep-01 deep-02 \
	tree-01 tree-02 \
	aliases-01 aliases-02 aliases-03 aliases-04 aliases-05 \
	aliases-06 aliases-07 aliases-08 aliases-09 \
	timing-01 \
	after-celem-base after-celem-01 after-celem-02 \
	pass-gates-01 pass-gates-02 pass-gates-03 \
	excl-rings-01 excl-rings-02 excl-rings-03 \
	check-excl-rings-01 check-excl-rings-02 check-excl-rings-03 \
	check-excl-rings-04 check-excl-rings-05

HACKT_PRSIM_CKPT_TEST_SCRIPTS_BASES = $(HACKT_PRSIMRC_TEST_SCRIPTS_BASES)

HACKT_PRSIM_OPT_TEST_SCRIPTS_BASES = $(HACKT_PRSIM_CKPT_TEST_SCRIPTS_BASES)

EXTRA_DIST += inv-13.prsimrc inv-14.prsimrc

include $(top_srcdir)/test/Make.test-tail

