# "test/lib/Makefile.am"
#	$Id: Makefile.am,v 1.24.4.7 2006/11/11 06:28:11 fang Exp $
# Test auto-Makefile for prsim.  

include $(top_srcdir)/test/Make.test-head

# tests in this directory will involve the lib directory files
HACO_FLAGS += -I$(top_srcdir)/lib

# see also examples in test/prsim

# just to make sure library files compile
HACKT_PARSE_TEST_PASSES = \
	logic_wrap \
	channel_wrap env_wrap buffer_wrap \
	alternator_wrap \
	e1of1-01 \
	source-01 \
	e1ofN-01 e1ofN-02 \
	logic-01

# where there are actual instantiations
# 20061017: removed these tests, too much of a pain to update
# will re-instate later with a better testing strategy.  
HACKT_UNROLL_TEST_PASSES =
HACKT_UNROLL_TEST_FAILURES =

HACKT_CREATE_TEST_PASSES = $(HACKT_UNROLL_TEST_PASSES)
HACKT_CREATE_TEST_FAILURES =

HACKT_ALLOC_TEST_PASSES = $(HACKT_CREATE_TEST_PASSES)

# .hac inputs (no suffix)
HACKT_PRSIMRC_TEST_SUBJECTS = \
	e1ofN-01 e1ofN-02 \
	logic-01 \
	env-source-sink-a-01 env-source-sink-a-02 \
	env-source-sink-e-01 env-source-sink-e-02 \
	alternator-onehot_counter-01 alternator-onehot_counter-02 \
	alternator-onehot_counter-03 alternator-onehot_counter-04 \
	alternator-onehot_counter-05 alternator-onehot_counter-06 \
	env-cmos-source-sink-e-01 env-cmos-source-sink-e-02 \
	env-cmos-source-sink-e-03 env-cmos-source-sink-e-04 \
	env-cmos-source-sink-e-05 env-cmos-source-sink-e-06 \
	env-cmos-source-sink-e-07 \
	env-cmos-source-sink-a-01 env-cmos-source-sink-a-02 \
	env-cmos-source-sink-a-03 env-cmos-source-sink-a-04 \
	env-cmos-source-sink-a-05 env-cmos-source-sink-a-06 \
	env-cmos-source-sink-a-07 \
	split-split_wchb_e1of2-01 split-split_wchb_e1of2-02 \
	merge-merge_wchb_e1of2-01 merge-merge_wchb_e1of2-02 \
	merge-merge_wchb_a1of2-01 merge-merge_wchb_a1of2-02 \
	alternator-1of2-01 alternator-1of2-02 \
	alternator-1of2-03 alternator-1of2-04 \
	alternator-wchb_eater-01 alternator-wchb_eater-02 \
	alternator-wchb_eater-03 alternator-wchb_eater-04 \
	alternator-wchb_eater-05 alternator-wchb_eater-06 \
	alternator-wchb_eater-07 alternator-wchb_eater-08 \
	alternator-wchb_eater-09 alternator-wchb_eater-10 \
	alternator-wchb_eater-11 alternator-wchb_eater-12 \
	alternator-wchb_repeater-01 alternator-wchb_repeater-02 \
	alternator-wchb_repeater-03 alternator-wchb_repeater-04 \
	alternator-wchb_repeater-05 alternator-wchb_repeater-06 \
	alternator-wchb_repeater-07 alternator-wchb_repeater-08 \
	alternator-wchb_split-01 alternator-wchb_split-02 \
	alternator-wchb_split-03 alternator-wchb_split-04 \
	alternator-wchb_split-05 alternator-wchb_split-06 \
	alternator-wchb_split-07 alternator-wchb_split-08 \
	alternator-wchb_split-09 alternator-wchb_split-10 \
	alternator-wchb_split-11 alternator-wchb_split-12 \
	alternator-wchb_split-13 alternator-wchb_split-14 \
	alternator-lock-01 alternator-lock-02 \
	alternator-bucket-01 \
	alternator-bucket-02 \
	alternator-bucket-03 \
	alternator-wchb_merge-01 alternator-wchb_merge-02 \
	alternator-wchb_merge-03 \
	alternator-wchb_merge-04 alternator-wchb_merge-05 \
	alternator-wchb_merge-06 alternator-wchb_merge-07 \
	alternator-wchb_merge-08 alternator-wchb_merge-09 \
	alternator-wchb_merge-10 alternator-wchb_merge-11 \
	alternator-wchb_tree_fifo-01 \
	alternator-wchb_tree_fifo-02 alternator-wchb_tree_fifo-03 \
	alternator-wchb_tree_fifo-04 \
	alternator-wchb_tree_fifo-05 alternator-wchb_tree_fifo-06 \
	alternator-wchb_tree_fifo-07 \
	copy-wire_copy-01 copy-wire_copy-02 \
	copy-wire_copy-03 copy-wire_copy-04 \
	copy-wchb_copy-01 copy-wchb_copy-02 \
	copy-wchb_copy-03 copy-wchb_copy-04

# .prsimrc inputs (no suffix)
HACKT_PRSIMRC_TEST_SCRIPTS_PASSES = \
	e1ofN-01 e1ofN-02 \
	logic-01 \
	env-source-sink-a-01 env-source-sink-a-02 \
	env-source-sink-e-01 env-source-sink-e-02 \
	alternator-onehot_counter-01 alternator-onehot_counter-02 \
	alternator-onehot_counter-03 alternator-onehot_counter-04 \
	alternator-onehot_counter-05 alternator-onehot_counter-06 \
	env-cmos-source-sink-e-01 env-cmos-source-sink-e-02 \
	env-cmos-source-sink-e-03 env-cmos-source-sink-e-04 \
	env-cmos-source-sink-e-05 env-cmos-source-sink-e-06 \
	env-cmos-source-sink-e-07 \
	env-cmos-source-sink-a-01 env-cmos-source-sink-a-02 \
	env-cmos-source-sink-a-03 env-cmos-source-sink-a-04 \
	env-cmos-source-sink-a-05 env-cmos-source-sink-a-06 \
	env-cmos-source-sink-a-07 \
	split-split_wchb_e1of2-01 split-split_wchb_e1of2-02 \
	merge-merge_wchb_e1of2-01 merge-merge_wchb_e1of2-02 \
	merge-merge_wchb_a1of2-01 merge-merge_wchb_a1of2-02 \
	alternator-1of2-01 alternator-1of2-02 \
	alternator-1of2-03 alternator-1of2-04 \
	alternator-wchb_eater-01 alternator-wchb_eater-02 \
	alternator-wchb_eater-03 alternator-wchb_eater-04 \
	alternator-wchb_eater-05 alternator-wchb_eater-06 \
	alternator-wchb_eater-07 alternator-wchb_eater-08 \
	alternator-wchb_eater-09 alternator-wchb_eater-10 \
	alternator-wchb_eater-11 alternator-wchb_eater-12 \
	alternator-wchb_repeater-01 alternator-wchb_repeater-02 \
	alternator-wchb_repeater-03 alternator-wchb_repeater-04 \
	alternator-wchb_repeater-05 alternator-wchb_repeater-06 \
	alternator-wchb_repeater-07 alternator-wchb_repeater-08 \
	alternator-wchb_split-01 alternator-wchb_split-02 \
	alternator-wchb_split-03 alternator-wchb_split-04 \
	alternator-wchb_split-05 alternator-wchb_split-06 \
	alternator-wchb_split-07 alternator-wchb_split-08 \
	alternator-wchb_split-09 alternator-wchb_split-10 \
	alternator-wchb_split-11 alternator-wchb_split-12 \
	alternator-wchb_split-13 alternator-wchb_split-14 \
	alternator-lock-01 alternator-lock-02 \
	alternator-bucket-01 \
	alternator-bucket-02 \
	alternator-bucket-03 \
	alternator-wchb_merge-01 alternator-wchb_merge-02 \
	alternator-wchb_merge-03 \
	alternator-wchb_merge-04 alternator-wchb_merge-05 \
	alternator-wchb_merge-06 alternator-wchb_merge-07 \
	alternator-wchb_merge-08 alternator-wchb_merge-09 \
	alternator-wchb_merge-10 alternator-wchb_merge-11 \
	alternator-wchb_tree_fifo-01 \
	alternator-wchb_tree_fifo-02 alternator-wchb_tree_fifo-03 \
	alternator-wchb_tree_fifo-04 \
	alternator-wchb_tree_fifo-05 alternator-wchb_tree_fifo-06 \
	alternator-wchb_tree_fifo-07 \
	copy-wire_copy-01 copy-wire_copy-02 \
	copy-wire_copy-03 copy-wire_copy-04 \
	copy-wchb_copy-01 copy-wchb_copy-02 \
	copy-wchb_copy-03 copy-wchb_copy-04

HACKT_PRSIMRC_TEST_SCRIPTS_FAILURES =

HACKT_CFLAT_LVS_TEST_PASSES = \
	nand-lvs-01

HACKT_PRSIM_CKPT_TEST_SCRIPTS_BASES = $(HACKT_PRSIMRC_TEST_SCRIPTS_PASSES)
HACKT_PRSIM_OPT_TEST_SCRIPTS_BASES = $(HACKT_PRSIM_CKPT_TEST_SCRIPTS_BASES)

srcdir_EXTRA_DIST += README \
	reset-test.prsimrc \
	reset-test-short.prsimrc \
	reset-test-medium.prsimrc \
	stable-test-short.prsimrc \
	stable-test-medium.prsimrc

# unprocessed test cases
# srcdir_EXTRA_DIST +=

include $(top_srcdir)/test/Make.test-tail

