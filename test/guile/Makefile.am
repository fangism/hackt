# "test/chpsim/Makefile.am"
#	$Id: Makefile.am,v 1.1.2.1 2007/04/13 01:55:10 fang Exp $
# Test auto-Makefile for chpsim.  

include $(top_srcdir)/test/Make.test-head

SUBDIRS =

HACO_FLAGS +=

HACKT_PARSE_TEST_PASSES = empty

# in this directory, no parse-fail tests
HACKT_CREATE_TEST_PASSES = $(HACKT_PARSE_TEST_PASSES)

HACKT_ALLOC_TEST_PASSES = $(HACKT_CREATE_TEST_PASSES)

# temporary for now:
HACKT_CHPSIM_ALLOC_TEST_SUBJECTS = $(HACKT_ALLOC_TEST_PASSES)

# TODO: eventually generate object test cases for all of these:
HACKT_CHPSIMRC_TEST_SUBJECTS =

# consider making .chpsim-event-dot/ps for visualizations

HACKT_CHPSIMRC_TEST_SCRIPTS_PASSES =

HACKT_CHPSIMRC_TEST_SCRIPTS_FAILURES =

# not yet, three tests report checkpoint diffs due to queue-reorderings
HACKT_CHPSIM_CKPT_TEST_SCRIPTS_BASES = $(HACKT_CHPSIMRC_TEST_SCRIPTS_PASSES)

HACKT_GUILE_TEST_SCRIPTS_PASSES = \
	rb-tree-01 rb-tree-02

HACKT_GUILE_TEST_SCRIPTS_FAILURES =

# additional sources not already mentioned
# srcdir_EXTRA_DIST +=

include $(top_srcdir)/test/Make.test-tail

