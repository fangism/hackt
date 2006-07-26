# "test/lib/Makefile.am"
#	$Id: Makefile.am,v 1.4 2006/07/26 19:27:46 fang Exp $
# Test auto-Makefile for prsim.  

include $(top_srcdir)/test/Make.test-head

# tests in this directory will involve the lib directory files
HACO_FLAGS += -I$(top_srcdir)/lib

# see also examples in test/prsim

# just to make sure library files compile
HACKT_PARSE_TEST_PASSES = \
	logic_wrap \
	channel_wrap env_wrap buffer_wrap \
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
	logic-01

# .prsimrc inputs (no suffix)
HACKT_PRSIMRC_TEST_SCRIPTS_BASES = \
	e1ofN-01 e1ofN-02 \
	logic-01

HACKT_PRSIM_CKPT_TEST_SCRIPTS_BASES = $(HACKT_PRSIMRC_TEST_SCRIPTS_BASES)
HACKT_PRSIM_OPT_TEST_SCRIPTS_BASES = $(HACKT_PRSIM_CKPT_TEST_SCRIPTS_BASES)

EXTRA_DIST += README

# unprocessed test cases
EXTRA_DIST +=

include $(top_srcdir)/test/Make.test-tail

