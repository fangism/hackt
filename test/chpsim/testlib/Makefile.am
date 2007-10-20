# "test/chpsim/testlib/Makefile.am"
#	$Id: Makefile.am,v 1.11.2.3 2007/10/20 21:14:18 fang Exp $
# Test auto-Makefile for chpsim.  

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_PASSES = \
	bool-and bool-or bool-xor \
	bool-copy bool-source bool-sink \
	bool-sink-alt \
	bool-split bool-merge \
	bool-copy-test \
	bool-table bool-memory \
	bool-buf \
	bool-peekbuf \
	bool-alternator \
	bool-peekbuf-test \
	bool-sink-alt-test \
	int-source int-sink \
	int-table \
	int-buf int-copy \
	int-arith \
	channel-sync channel-desync \
	fibonacci \
	void-sink void-source void-buf \
	round-robin round-robin-test \
	bitrouter bitrouter-dec \
	bitrouter-env bitrouter-test \
	quad-bitrouter

HACKT_CREATE_TEST_PASSES = \
	bool-copy-test \
	bool-peekbuf-test \
	bool-sink-alt-test \
	round-robin-test \
	bitrouter-test

HACKT_ALLOC_TEST_PASSES = $(HACKT_CREATE_TEST_PASSES)

HACKT_CHPSIM_ALLOC_TEST_SUBJECTS = $(HACKT_ALLOC_TEST_PASSES)

HACKT_CHPSIMRC_TEST_SUBJECTS =

HACKT_CHPSIMRC_TEST_SCRIPTS_PASSES =

# HACKT_CHPSIMRC_TEST_SCRIPTS_FAILURES =

HACKT_CHPSIM_GUILE_TEST_SCRIPTS_PASSES = \
	bool-copy-test-01

# additional sources not already mentioned
srcdir_EXTRA_DIST += README

include $(top_srcdir)/test/Make.test-tail

