# "test/prsim/Makefile.am"
#	$Id: Makefile.am,v 1.5.6.4 2006/05/06 02:13:46 fang Exp $
# Test auto-Makefile for prsim.  

include $(top_srcdir)/test/Make.test-head

# .hac inputs
HACKT_PRSIMRC_TEST_SUBJECTS = celem inv oscillator half-celem deep tree \
	celem-share after-celem-def after-celem after-celem-more \
	pass-gates-def pass-gates excl-rings-def excl-rings

HACKT_PRSIMRC_TEST_SCRIPTS_BASES = \
	celem-01 celem-02 celem-03 celem-04 \
	inv-00 inv-01 inv-02 inv-03 inv-04 inv-05 inv-06 inv-07 inv-08 inv-09 \
	cycle-source-a cycle-source-b \
	unstable-01 \
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
	excl-rings-01 excl-rings-02 excl-rings-03

HACKT_PRSIM_CKPT_TEST_SCRIPTS_BASES = $(HACKT_PRSIMRC_TEST_SCRIPTS_BASES)

HACKT_PRSIM_OPT_TEST_SCRIPTS_BASES = $(HACKT_PRSIM_CKPT_TEST_SCRIPTS_BASES)

EXTRA_DIST +=

include $(top_srcdir)/test/Make.test-tail

