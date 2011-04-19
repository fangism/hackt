# "test/prsim/Makefile.am"
#	$Id: Makefile.am,v 1.1.2.2 2011/04/19 22:31:32 fang Exp $
# Test auto-Makefile for prsim.  

include $(top_srcdir)/test/Make.test-head

# SUBDIRS = .

HACKT_IPPLERC_TEST_SCRIPTS_PASSES = \
	001 002 003 003b 004 004b 005 006

# random tests have non-zero temperature for randomness
if RANDOM_TESTS
else
006.ipltest: 006.iplrc
	$(SKIP_TEST_SCRIPT)
endif

include $(top_srcdir)/test/Make.test-tail

