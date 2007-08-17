# "test/quarantine/Makefile.am"
#	$Id: Makefile.am,v 1.5 2007/08/17 00:46:49 fang Exp $

include $(top_srcdir)/test/Make.test-head

# tests in this directory will involve the lib directory files
HACO_FLAGS += -I$(top_srcdir)/lib

# just to make sure library files compile
HACKT_PARSE_TEST_PASSES =
HACKT_PARSE_TEST_WARNINGS =
HACKT_PARSE_TEST_FAILURES =

# TODO: redefine in terms of PASSES and FAILURES
HACKT_CREATE_TEST_PASSES = $(HACKT_PARSE_TEST_PASSES)
HACKT_ALLOC_TEST_PASSES = $(HACKT_CREATE_TEST_PASSES)

HACKT_PRSIMRC_TEST_SUBJECTS =

HACKT_PRSIMRC_TEST_SCRIPTS_PASSES =

HACKT_CFLAT_LVS_TEST_PASSES =
HACKT_CFLAT_LVS_TEST_FAILURES =

HACKT_PRSIM_CKPT_TEST_SCRIPTS_BASES = $(HACKT_PRSIMRC_TEST_SCRIPTS_BASES)
HACKT_PRSIM_OPT_TEST_SCRIPTS_BASES = $(HACKT_PRSIM_CKPT_TEST_SCRIPTS_BASES)

# add test-cases here
srcdir_EXTRA_DIST += README \
	mem-01.hac \
	bad_close.hac

include $(top_srcdir)/test/Make.test-tail

