# "test/chpsim/testlib/Makefile.am"
#	$Id: Makefile.am,v 1.3.2.2 2007/01/27 23:39:25 fang Exp $
# Test auto-Makefile for chpsim.  

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_PASSES = \
	bool-and bool-or bool-xor \
	bool-copy bool-source bool-sink \
	bool-split bool-merge \
	bool-copy-test \
	bool-table bool-memory \
	int-source int-sink \
	int-table \
	channel-sync channel-desync

HACKT_CREATE_TEST_PASSES = \
	bool-copy-test

HACKT_ALLOC_TEST_PASSES = $(HACKT_CREATE_TEST_PASSES)

HACKT_CHPSIM_ALLOC_TEST_SUBJECTS = $(HACKT_ALLOC_TEST_PASSES)

HACKT_CHPSIMRC_TEST_SUBJECTS =

HACKT_CHPSIMRC_TEST_SCRIPTS_PASSES =

# HACKT_CHPSIMRC_TEST_SCRIPTS_FAILURES =

# additional sources not already mentioned
srcdir_EXTRA_DIST += README

include $(top_srcdir)/test/Make.test-tail

