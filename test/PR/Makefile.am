# "test/prsim/Makefile.am"
#	$Id: Makefile.am,v 1.1.2.10 2011/04/27 20:57:34 fang Exp $
# Test auto-Makefile for prsim.  

include $(top_srcdir)/test/Make.test-head

# SUBDIRS = .

HACKT_IPPLERC_TEST_SCRIPTS_PASSES = \
	001 002 003 003b 004 004b 005 005b 005c 006 006b 007 008 008b 009 \
	010 011 012 013 014 015 015b 016 016b 017 017a 017b 017c 017d \
	018 018a 018b 019 019a 019b 019c 019d 019e 019f 019g

# random tests have non-zero temperature for randomness
if RANDOM_TESTS
else
006.ipltest: 006.iplrc
	$(SKIP_TEST_SCRIPT)
endif

HACKT_IPPLE_CKPT_TEST_SCRIPTS_BASES = $(HACKT_IPPLERC_TEST_SCRIPTS_PASSES)

include $(top_srcdir)/test/Make.test-tail

