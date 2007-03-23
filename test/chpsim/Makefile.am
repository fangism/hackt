# "test/chpsim/Makefile.am"
#	$Id: Makefile.am,v 1.8.2.5 2007/03/23 23:16:28 fang Exp $
# Test auto-Makefile for chpsim.  

include $(top_srcdir)/test/Make.test-head

SUBDIRS = testlib .

HACO_FLAGS += -I$(srcdir)/testlib

HACKT_PARSE_TEST_PASSES = \
	do-while-countdown \
	countdown-successor \
	countdown-loop-a countdown-loop-b \
	countdown-nested-a countdown-nested-b \
	countdown-nested-c countdown-nested-d \
	countdown-domino-a countdown-domino-b \
	wait-basic-a \
	wait-conjunction-a wait-conjunction-b wait-conjunction-c \
	wait-indexed-guard-a \
 	select-skip-a select-skip-b \
	select-sequence-a \
	select-parallel-a \
	select-loop-a select-loop-b \
	arbitrate-skip-a arbitrate-skip-b \
	arbitrate-loop-a \
	select-nested-loop-a select-nested-loop-b \
	select-fail-a \
	select-block-a \
	bool-and-test-a \
	bool-or-test-a \
	bool-xor-test-a \
	channel-probe-a channel-probe-b channel-probe-c

# in this directory, no parse-fail tests
HACKT_CREATE_TEST_PASSES = $(HACKT_PARSE_TEST_PASSES)

HACKT_ALLOC_TEST_PASSES = $(HACKT_CREATE_TEST_PASSES)

# temporary for now:
HACKT_CHPSIM_ALLOC_TEST_SUBJECTS = $(HACKT_ALLOC_TEST_PASSES)

# TODO: eventually generate object test cases for all of these:
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
	do-while-countdown \
	countdown-sequence-a \
	countdown-parallel-a \
	bool-split-a bool-split-b \
	bool-split-select-a bool-split-select-b \
	bool-lookup-a bool-lookup-b \
	channel-sync-desync-a \
	bool-memory-read-a bool-memory-read-b \
	bool-memory-write-a bool-memory-write-b \
	buffer-advance-to \
	delayed-source-a \
	delayed-sink-a \
	bool-and-test-b

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
	channel-once-parallel-a-02 \
	channel-self-sequence-01 \
	channel-self-parallel-a-01 channel-self-parallel-b-01 \
	channel-self-parallel-a-02 channel-self-parallel-b-02 \
	do-while-countdown-01 do-while-countdown-02 \
	do-while-countdown-03-watch \
	do-while-countdown-04-break \
	do-while-countdown-05-break \
	do-while-countdown-06-watch \
	do-while-countdown-07-break \
	countdown-successor-01 \
	countdown-loop-a-01 countdown-loop-b-01 \
	countdown-sequence-a-01 \
	countdown-nested-a-01 countdown-nested-a-02 \
	countdown-nested-b-01 countdown-nested-b-02 \
	countdown-nested-c-01 countdown-nested-c-02 \
	countdown-nested-d-01 countdown-nested-d-02 \
	countdown-domino-a-01 \
	countdown-domino-b-01 \
	countdown-domino-b-02-cause countdown-domino-b-03-cause \
	wait-basic-a-01 wait-basic-a-02 \
	wait-conjunction-a-01 wait-conjunction-a-02 \
	wait-conjunction-b-01 wait-conjunction-b-02 \
	wait-conjunction-c-01 wait-conjunction-c-02 \
	wait-indexed-guard-a-01 wait-indexed-guard-a-02 \
	select-skip-a-01 \
	select-skip-b-01 \
	select-sequence-a-01 select-sequence-a-02-cause \
	select-parallel-a-01 select-parallel-a-02 \
	select-parallel-a-03 select-parallel-a-04 \
	select-loop-a-01 \
	select-loop-b-01 \
	arbitrate-skip-a-01 \
	arbitrate-skip-b-01 \
	arbitrate-loop-a-01 arbitrate-loop-a-02 arbitrate-loop-a-03 \
	select-nested-loop-a-01 select-nested-loop-a-02 \
	select-nested-loop-b-01 select-nested-loop-b-02 \
	select-block-a-01 select-block-a-02 \
	bool-split-a-01 \
	bool-split-select-a-01 \
	bool-lookup-a-01 \
	channel-sync-desync-a-01 channel-sync-desync-a-02 \
	channel-sync-desync-a-03-trace \
	channel-sync-desync-a-04-trace \
	channel-sync-desync-a-05-trace \
	bool-memory-read-a-01 \
	bool-memory-write-a-01 \
	bool-and-test-a-01 \
	bool-and-test-a-02-break \
	bool-and-test-b-01 bool-and-test-b-02-trace \
	bool-or-test-a-01 \
	bool-xor-test-a-01 \
	buffer-advance-to \
	channel-probe-a-01 channel-probe-b-01 channel-probe-c-01 \
	delayed-source-a-01 delayed-source-a-02-trace \
	delayed-sink-a-01 delayed-sink-a-02-trace

HACKT_CHPSIMRC_TEST_SCRIPTS_FAILURES = \
	select-fail-a-01 \
	bool-split-b-01 \
	bool-split-select-b-01 \
	bool-lookup-b-01 \
	bool-memory-read-b-01 \
	bool-memory-write-b-01

# not yet, three tests report checkpoint diffs due to queue-reorderings
HACKT_CHPSIM_CKPT_TEST_SCRIPTS_BASES = $(HACKT_CHPSIMRC_TEST_SCRIPTS_PASSES)

HACKT_CHPSIM_GUILE_TEST_SCRIPTS_PASSES = \
	channel-sync-desync-a-01 channel-sync-desync-a-02 \
	channel-sync-desync-a-03 channel-sync-desync-a-04 \
	channel-sync-desync-a-05 channel-sync-desync-a-06 \
	channel-sync-desync-a-07 channel-sync-desync-a-08 \
	channel-sync-desync-a-09 channel-sync-desync-a-10 \
	channel-sync-desync-a-11

HACKT_CHPSIM_GUILE_TEST_SCRIPTS_FAILURES =

# additional sources not already mentioned
# srcdir_EXTRA_DIST +=

# Additional dependencies from known-side effects
channel-sync-desync-a-04.chpsimtrace: channel-sync-desync-a-04-trace.chpsimrc-out
	@:

channel-sync-desync-a-05.chpsimtrace: channel-sync-desync-a-05-trace.chpsimrc-out
	@:

include $(top_srcdir)/test/Make.test-tail

