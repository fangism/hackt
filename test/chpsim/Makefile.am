# "test/chpsim/Makefile.am"
#	$Id: Makefile.am,v 1.1.2.2 2007/01/17 04:46:31 fang Exp $
# Test auto-Makefile for chpsim.  

include $(top_srcdir)/test/Make.test-head

HACKT_CHPSIMRC_TEST_SUBJECTS = \
	assign-once \
	assign-once-parallel-a assign-once-parallel-b \
	assign-once-parallel-c assign-once-parallel-d \
	assign-once-sequence-a assign-once-sequence-b \
	send-once \
	receive-once \
	channel-once-parallel-a channel-once-parallel-b \
	channel-self-sequence \
	channel-self-parallel-a channel-self-parallel-b

# consider making .chpsim-event-dot/ps for visualizations

HACKT_CHPSIMRC_TEST_SCRIPTS_PASSES = \
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
	channel-self-parallel-a-01 channel-self-parallel-b-01


# additional sources not already mentioned
# srcdir_EXTRA_DIST +=

include $(top_srcdir)/test/Make.test-tail

