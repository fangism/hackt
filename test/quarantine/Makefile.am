# "test/quarantine/Makefile.am"
#	$Id: Makefile.am,v 1.6 2008/11/12 22:23:34 fang Exp $

include $(top_srcdir)/test/Make.test-head

# tests in this directory will involve the lib directory files
HACO_FLAGS += -I$(top_srcdir)/lib

# just to make sure library files compile
HACKT_PARSE_TEST_PASSES = \
	mem-01
HACKT_PARSE_TEST_WARNINGS =
HACKT_PARSE_TEST_FAILURES =

# TODO: redefine in terms of PASSES and FAILURES
HACKT_CREATE_TEST_PASSES =
# $(HACKT_PARSE_TEST_PASSES)
HACKT_ALLOC_TEST_PASSES =
# $(HACKT_CREATE_TEST_PASSES)

HACKT_PRSIMRC_TEST_SUBJECTS = \
	mem-01

HACKT_PRSIMRC_TEST_SCRIPTS_PASSES = \
	memstats32-mem-01 memstats64-mem-01

if ABI_ILP32
memstats64-mem-01.prsimtest: memstats64-mem-01.prsimrc
	$(SKIP_TEST_SCRIPT)
else
memstats32-mem-01.prsimtest: memstats32-mem-01.prsimrc
	$(SKIP_TEST_SCRIPT)
endif

HACKT_CFLAT_LVS_TEST_PASSES =
HACKT_CFLAT_LVS_TEST_FAILURES =

HACKT_PRSIM_CKPT_TEST_SCRIPTS_BASES = $(HACKT_PRSIMRC_TEST_SCRIPTS_BASES)
HACKT_PRSIM_OPT_TEST_SCRIPTS_BASES = $(HACKT_PRSIM_CKPT_TEST_SCRIPTS_BASES)

# add test-cases here
srcdir_EXTRA_DIST += README \
	bad_close.hac

include $(top_srcdir)/test/Make.test-tail

