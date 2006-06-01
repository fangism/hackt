# "test/lib/Makefile.am"
#	$Id: Makefile.am,v 1.2 2006/06/01 21:27:50 fang Exp $
# Test auto-Makefile for prsim.  

include $(top_srcdir)/test/Make.test-head

# tests in this directory will involve the lib directory files
HACO_FLAGS += -I$(top_srcdir)/lib

# see also examples in test/prsim

# just to make sure library files compile
HACKT_PARSE_TEST_PASSES = \
	channel_wrap env_wrap buffer_wrap

HACKT_PARSE_TEST_SUBJECTS = \
	$(HACKT_PARSE_TEST_FAILURES) \
	$(HACKT_PARSE_TEST_PASSES)

HACKT_OBJ_TEST_SUBJECTS = \
	$(HACKT_PARSE_TEST_PASSES)

# where there are actual instantiations
# HACKT_UNROLL_TEST_SUBJECTS =
# HACKT_CREATE_TEST_SUBJECTS =
# HACKT_ALLOC_TEST_SUBJECTS =

# .hac inputs (no suffix)
HACKT_PRSIMRC_TEST_SUBJECTS =

# .prsimrc inputs (no suffix)
HACKT_PRSIMRC_TEST_SCRIPTS_BASES =

# add back later...
# HACKT_PRSIM_CKPT_TEST_SCRIPTS_BASES = $(HACKT_PRSIMRC_TEST_SCRIPTS_BASES)

# add back later...
# HACKT_PRSIM_OPT_TEST_SCRIPTS_BASES = $(HACKT_PRSIM_CKPT_TEST_SCRIPTS_BASES)

EXTRA_DIST += README

# unprocessed test cases
EXTRA_DIST += source-01.hac

include $(top_srcdir)/test/Make.test-tail

