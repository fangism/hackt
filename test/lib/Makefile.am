# "test/lib/Makefile.am"
#	$Id: Makefile.am,v 1.14 2006/09/14 05:50:07 fang Exp $
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

HACKT_PARSE_TEST_SUBJECTS = \
	$(HACKT_PARSE_TEST_FAILURES) \
	$(HACKT_PARSE_TEST_PASSES)

HACKT_OBJ_TEST_SUBJECTS = \
	$(HACKT_PARSE_TEST_PASSES)

# where there are actual instantiations
HACKT_UNROLL_TEST_SUBJECTS = $(HACKT_PARSE_TEST_PASSES)
HACKT_CREATE_TEST_SUBJECTS = $(HACKT_UNROLL_TEST_SUBJECTS)
HACKT_ALLOC_TEST_SUBJECTS = $(HACKT_CREATE_TEST_SUBJECTS)

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
	alternator-1of2-01 alternator-1of2-02 \
	alternator-wchb_eater-01 alternator-wchb_eater-02 \
	alternator-wchb_eater-03 alternator-wchb_eater-04 \
	alternator-wchb_split-01 alternator-wchb_split-02 \
	alternator-wchb_split-03 alternator-wchb_split-04 \
	alternator-wchb_split-05 alternator-wchb_split-06 \
	alternator-lock-01

# .prsimrc inputs (no suffix)
HACKT_PRSIMRC_TEST_SCRIPTS_BASES = \
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
	alternator-1of2-01 alternator-1of2-02 \
	alternator-wchb_eater-01 alternator-wchb_eater-02 \
	alternator-wchb_eater-03 alternator-wchb_eater-04 \
	alternator-wchb_split-01 alternator-wchb_split-02 \
	alternator-wchb_split-03 alternator-wchb_split-04 \
	alternator-wchb_split-05 alternator-wchb_split-06 \
	alternator-lock-01

HACKT_CFLAT_LVS_TEST_SUBJECTS = \
	nand-lvs-01

HACKT_PRSIM_CKPT_TEST_SCRIPTS_BASES = $(HACKT_PRSIMRC_TEST_SCRIPTS_BASES)
HACKT_PRSIM_OPT_TEST_SCRIPTS_BASES = $(HACKT_PRSIM_CKPT_TEST_SCRIPTS_BASES)

EXTRA_DIST += README \
	stable-test-short.prsimrc \
	stable-test-medium.prsimrc

# unprocessed test cases
# EXTRA_DIST +=

include $(top_srcdir)/test/Make.test-tail

