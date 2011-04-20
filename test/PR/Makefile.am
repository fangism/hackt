# "test/prsim/Makefile.am"
#	$Id: Makefile.am,v 1.1.2.4 2011/04/20 01:50:13 fang Exp $
# Test auto-Makefile for prsim.  

include $(top_srcdir)/test/Make.test-head

# SUBDIRS = .

HACKT_IPPLERC_TEST_SCRIPTS_PASSES = \
	001 002 003 003b 004 004b 005 005b 006 006b

# random tests have non-zero temperature for randomness
if RANDOM_TESTS
else
006.ipltest: 006.iplrc
	$(SKIP_TEST_SCRIPT)
endif

HACKT_IPPLE_CKPT_TEST_SCRIPTS_BASES = $(HACKT_IPPLERC_TEST_SCRIPTS_PASSES)

include $(top_srcdir)/test/Make.test-tail

