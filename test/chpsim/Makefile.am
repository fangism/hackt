# "test/chpsim/Makefile.am"
#	$Id: Makefile.am,v 1.1.2.3 2007/01/18 12:45:54 fang Exp $
# Test auto-Makefile for chpsim.  

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_PASSES = \
	do-while-countdown \
	countdown-successor \
	countdown-loop-a countdown-loop-b

# in this directory, no parse-fail tests
HACKT_CREATE_TEST_PASSES = $(HACKT_PARSE_TEST_PASSES)

HACKT_ALLOC_TEST_PASSES = $(HACKT_CREATE_TEST_PASSES)

# temporary for now:
HACKT_CHPSIM_ALLOC_TEST_SUBJECTS = $(HACKT_ALLOC_TEST_PASSES)


HACKT_CHPSIMRC_TEST_SUBJECTS = \
	defaults \
	assign-once \
	assign-once-parallel-a assign-once-parallel-b \
	assign-once-parallel-c assign-once-parallel-d \
	assign-once-sequence-a assign-once-sequence-b \
	send-once \
	receive-once \
	channel-once-parallel-a channel-once-parallel-b \
	channel-self-sequence \
	channel-self-parallel-a channel-self-parallel-b \
	do-while-countdown

# consider making .chpsim-event-dot/ps for visualizations

HACKT_CHPSIMRC_TEST_SCRIPTS_PASSES = \
	defaults-only \
	assign-once-01 \
	assign-once-parallel-a-01 \
	assign-once-parallel-b-01 \
	assign-once-parallel-c-01 \
	assign-once-parallel-d-01 \
	assign-once-sequence-a-01 \
	assign-once-sequence-b-01 \
	send-once-01 \
	receive-once-01 \
	channel-once-parallel-a-01 channel-once-parallel-b-01 \
	channel-self-sequence-01 \
	channel-self-parallel-a-01 channel-self-parallel-b-01 \
	channel-self-parallel-a-02 channel-self-parallel-b-02 \
	do-while-countdown-01 do-while-countdown-02 \
	countdown-successor-01


# additional sources not already mentioned
# srcdir_EXTRA_DIST +=

include $(top_srcdir)/test/Make.test-tail

