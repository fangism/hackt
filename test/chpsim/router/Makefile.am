# "test/chpsim/router/Makefile.am"
#	$Id: Makefile.am,v 1.5.8.1 2009/01/30 19:23:44 fang Exp $
# Test auto-Makefile for chpsim.  

include $(top_srcdir)/test/Make.test-head
# include $(top_srcdir)/installdirs.mk
# for examplesdir

# SUBDIRS = .

HACO_FLAGS += -I$(srcdir)/../testlib -I$(srcdir)/..
CHPSIM_LIB_PATH = ../../../lib

# list tests here only if they are also intended for create/alloc tests
HACKT_PARSE_TEST_PASSES =

# in this directory, no parse-fail tests
HACKT_CREATE_TEST_PASSES = $(HACKT_PARSE_TEST_PASSES)

HACKT_ALLOC_TEST_PASSES = $(HACKT_CREATE_TEST_PASSES)

# temporary for now:
HACKT_CHPSIM_ALLOC_TEST_SUBJECTS = $(HACKT_ALLOC_TEST_PASSES)

# list files here that are HAC sources used in chpsim testing
# NOTE: tests related to the CHP standard library should be prefixed 'lib-'
# TODO: eventually generate object test cases for all of these:
HACKT_CHPSIMRC_TEST_SUBJECTS = \
	bitrouter-seq-test-channels \
	bitrouter-seq-a \
	bitrouter-dec-a \
	bitrouter-dec-b \
	bitrouter-twin-a \
	bitrouter-seq-test-sinks-ideal \
	bitrouter-seq-test-env-01 bitrouter-seq-test-env-02 \
	bitrouter-seq-test-env-03 bitrouter-seq-test-env-04 \
	bitrouter-seq-test-env-05 bitrouter-seq-test-env-06 \
	bitrouter-seq-test-env-07 \
	bitrouter-seq-test-env-08 bitrouter-seq-test-env-09 \
	bitrouter-seq-test-a-01 bitrouter-seq-test-a-02 \
	bitrouter-dec-test-a-01 bitrouter-dec-test-a-02 \
	bitrouter-dec-test-a-03 bitrouter-dec-test-a-04 \
	bitrouter-dec-test-a-05 bitrouter-dec-test-a-06 \
	bitrouter-dec-test-a-07 \
	bitrouter-dec-test-a-08 bitrouter-dec-test-a-09 \
	bitrouter-dec-test-b-01 \
	bitrouter-twin-test-a-01 bitrouter-twin-test-a-02 \
	bitrouter-twin-test-a-03 bitrouter-twin-test-a-04 \
	bitrouter-twin-test-a-05 bitrouter-twin-test-a-06 \
	bitrouter-twin-test-a-07 \
	bitrouter-twin-test-a-08 bitrouter-twin-test-a-09 \
	quad-bitrouter-mmss quad-bitrouter-msms quad-bitrouter-mssm \
	quad-bitrouter-smms quad-bitrouter-smsm quad-bitrouter-ssmm \
	quad-bitrouter-test-channels \
	quad-bitrouter-test-source-lib \
	quad-bitrouter-test-source-01 quad-bitrouter-test-source-02 \
	quad-bitrouter-test-source-03 quad-bitrouter-test-source-04 \
	quad-bitrouter-test-source-05 \
	quad-bitrouter-test-source-06 quad-bitrouter-test-source-07 \
	quad-bitrouter-test-source-08 quad-bitrouter-test-source-09 \
	quad-bitrouter-test-source-10 \
	quad-bitrouter-test-sinks-ideal \
	quad-bitrouter-test-env-01 quad-bitrouter-test-env-02 \
	quad-bitrouter-test-env-03 quad-bitrouter-test-env-04 \
	quad-bitrouter-test-env-05 \
	quad-bitrouter-test-env-06 quad-bitrouter-test-env-07 \
	quad-bitrouter-test-env-08 quad-bitrouter-test-env-09 \
	quad-bitrouter-test-env-10 \
	quad-bitrouter-mmss-test-01 quad-bitrouter-ssmm-test-01 \
	quad-bitrouter-mmss-test-02 quad-bitrouter-ssmm-test-02 \
	quad-bitrouter-mmss-test-03 quad-bitrouter-msms-test-03 \
	quad-bitrouter-mssm-test-03 quad-bitrouter-smms-test-03 \
	quad-bitrouter-smsm-test-03 quad-bitrouter-ssmm-test-03 \
	quad-bitrouter-mmss-test-04 quad-bitrouter-msms-test-04 \
	quad-bitrouter-mssm-test-04 quad-bitrouter-smms-test-04 \
	quad-bitrouter-smsm-test-04 quad-bitrouter-ssmm-test-04 \
	quad-bitrouter-mmss-test-05 quad-bitrouter-msms-test-05 \
	quad-bitrouter-mssm-test-05 quad-bitrouter-smms-test-05 \
	quad-bitrouter-smsm-test-05 quad-bitrouter-ssmm-test-05 \
	quad-bitrouter-mmss-test-06 quad-bitrouter-msms-test-06 \
	quad-bitrouter-mssm-test-06 quad-bitrouter-smms-test-06 \
	quad-bitrouter-smsm-test-06 quad-bitrouter-ssmm-test-06 \
	quad-bitrouter-mmss-test-07 quad-bitrouter-msms-test-07 \
	quad-bitrouter-mssm-test-07 quad-bitrouter-smms-test-07 \
	quad-bitrouter-smsm-test-07 quad-bitrouter-ssmm-test-07 \
	quad-bitrouter-mmss-test-08 quad-bitrouter-msms-test-08 \
	quad-bitrouter-mssm-test-08 quad-bitrouter-smms-test-08 \
	quad-bitrouter-smsm-test-08 quad-bitrouter-ssmm-test-08 \
	quad-bitrouter-mmss-test-09 quad-bitrouter-msms-test-09 \
	quad-bitrouter-mssm-test-09 quad-bitrouter-smms-test-09 \
	quad-bitrouter-smsm-test-09 quad-bitrouter-ssmm-test-09 \
	quad-bitrouter-mmss-test-10 quad-bitrouter-msms-test-10 \
	quad-bitrouter-mssm-test-10 quad-bitrouter-smms-test-10 \
	quad-bitrouter-smsm-test-10 quad-bitrouter-ssmm-test-10

# consider making .chpsim-event-dot/ps for visualizations

HACKT_CHPSIMRC_RANDOM_TEST_SCRIPTS_PASSES = \
	quad-bitrouter-msms-test-03 \
	quad-bitrouter-msms-test-06 \
	quad-bitrouter-msms-test-08 \
	quad-bitrouter-msms-test-09 \
	quad-bitrouter-msms-test-10 \
	quad-bitrouter-mssm-test-05 \
	quad-bitrouter-mssm-test-06 \
	quad-bitrouter-mssm-test-08 \
	quad-bitrouter-mssm-test-09 \
	quad-bitrouter-mssm-test-10

HACKT_CHPSIMRC_TEST_SCRIPTS_PASSES = \
	bitrouter-seq-test-a-01 bitrouter-seq-test-a-02 \
	bitrouter-dec-test-a-01 bitrouter-dec-test-a-02 \
	bitrouter-dec-test-a-03 bitrouter-dec-test-a-04 \
	bitrouter-dec-test-a-05 bitrouter-dec-test-a-06 \
	bitrouter-dec-test-a-07 \
	bitrouter-dec-test-a-08 bitrouter-dec-test-a-09 \
	bitrouter-dec-test-b-01 \
	bitrouter-twin-test-a-01 bitrouter-twin-test-a-02 \
	bitrouter-twin-test-a-03 bitrouter-twin-test-a-04 \
	bitrouter-twin-test-a-05 bitrouter-twin-test-a-06 \
	bitrouter-twin-test-a-07 \
	bitrouter-twin-test-a-08 bitrouter-twin-test-a-09 \
	quad-bitrouter-mmss-test-01 quad-bitrouter-ssmm-test-01 \
	quad-bitrouter-mmss-test-02 quad-bitrouter-ssmm-test-02 \
	quad-bitrouter-mmss-test-03 \
	quad-bitrouter-mssm-test-03 quad-bitrouter-smms-test-03 \
	quad-bitrouter-smsm-test-03 quad-bitrouter-ssmm-test-03 \
	quad-bitrouter-mmss-test-04 quad-bitrouter-msms-test-04 \
	quad-bitrouter-mssm-test-04 quad-bitrouter-smms-test-04 \
	quad-bitrouter-smsm-test-04 quad-bitrouter-ssmm-test-04 \
	quad-bitrouter-mmss-test-05 quad-bitrouter-msms-test-05 \
	quad-bitrouter-smms-test-05 \
	quad-bitrouter-smsm-test-05 quad-bitrouter-ssmm-test-05 \
	quad-bitrouter-mmss-test-06 \
	quad-bitrouter-smms-test-06 \
	quad-bitrouter-smsm-test-06 quad-bitrouter-ssmm-test-06 \
	quad-bitrouter-mmss-test-07 quad-bitrouter-msms-test-07 \
	quad-bitrouter-mssm-test-07 quad-bitrouter-smms-test-07 \
	quad-bitrouter-smsm-test-07 quad-bitrouter-ssmm-test-07 \
	quad-bitrouter-mmss-test-08 \
	quad-bitrouter-smms-test-08 \
	quad-bitrouter-smsm-test-08 quad-bitrouter-ssmm-test-08 \
	quad-bitrouter-mmss-test-09 \
	quad-bitrouter-smms-test-09 \
	quad-bitrouter-smsm-test-09 quad-bitrouter-ssmm-test-09 \
	quad-bitrouter-mmss-test-10 \
	quad-bitrouter-smms-test-10 \
	quad-bitrouter-smsm-test-10 quad-bitrouter-ssmm-test-10

# TODO: parallel-fifo test cases

HACKT_CHPSIMRC_TEST_SCRIPTS_FAILURES =

# not yet, three tests report checkpoint diffs due to queue-reorderings
HACKT_CHPSIM_CKPT_TEST_SCRIPTS_BASES = $(HACKT_CHPSIMRC_TEST_SCRIPTS_PASSES)

if RANDOM_TESTS
HACKT_CHPSIM_CKPT_TEST_SCRIPTS_BASES += \
	$(HACKT_CHPSIMRC_RANDOM_TEST_SCRIPTS_PASSES)
endif

HACKT_CHPSIM_GUILE_TEST_SCRIPTS_PASSES = \
	bitrouter-seq-test-a-01-trace-crit bitrouter-seq-test-a-02-trace-crit \
	bitrouter-dec-test-a-01-trace-crit bitrouter-dec-test-a-02-trace-crit \
	bitrouter-dec-test-a-03-trace-crit bitrouter-dec-test-a-04-trace-crit \
	bitrouter-dec-test-a-05-trace-crit bitrouter-dec-test-a-06-trace-crit \
	bitrouter-dec-test-a-07-trace-crit \
	bitrouter-twin-test-a-01-trace-crit \
	bitrouter-twin-test-a-02-trace-crit \
	bitrouter-twin-test-a-03-trace-crit \
	bitrouter-twin-test-a-04-trace-crit \
	bitrouter-twin-test-a-05-trace-crit \
	bitrouter-twin-test-a-06-trace-crit \
	bitrouter-twin-test-a-07-trace-crit \
	bitrouter-twin-test-a-08-trace-crit \
	bitrouter-twin-test-a-09-trace-crit
 
HACKT_CHPSIM_GUILE_RANDOM_TEST_SCRIPTS_PASSES = \
	bitrouter-dec-test-a-08-trace-crit bitrouter-dec-test-a-09-trace-crit \
	bitrouter-dec-test-b-01-trace-crit \
	quad-bitrouter-mmss-test-01-trace quad-bitrouter-mmss-test-02-trace \
	quad-bitrouter-mmss-test-03-trace quad-bitrouter-mmss-test-04-trace \
	quad-bitrouter-mmss-test-05-trace quad-bitrouter-mmss-test-06-trace \
	quad-bitrouter-mmss-test-07-trace quad-bitrouter-mmss-test-08-trace \
	quad-bitrouter-mmss-test-09-trace quad-bitrouter-mmss-test-10-trace \
	quad-bitrouter-msms-test-03-trace quad-bitrouter-msms-test-04-trace \
	quad-bitrouter-msms-test-05-trace quad-bitrouter-msms-test-06-trace \
	quad-bitrouter-msms-test-07-trace quad-bitrouter-msms-test-08-trace \
	quad-bitrouter-msms-test-09-trace quad-bitrouter-msms-test-10-trace \
	quad-bitrouter-mssm-test-03-trace quad-bitrouter-mssm-test-04-trace \
	quad-bitrouter-mssm-test-05-trace quad-bitrouter-mssm-test-06-trace \
	quad-bitrouter-mssm-test-07-trace quad-bitrouter-mssm-test-08-trace \
	quad-bitrouter-mssm-test-09-trace quad-bitrouter-mssm-test-10-trace \
	quad-bitrouter-smms-test-03-trace quad-bitrouter-smms-test-04-trace \
	quad-bitrouter-smms-test-05-trace quad-bitrouter-smms-test-06-trace \
	quad-bitrouter-smms-test-07-trace quad-bitrouter-smms-test-08-trace \
	quad-bitrouter-smms-test-09-trace quad-bitrouter-smms-test-10-trace \
	quad-bitrouter-smsm-test-03-trace quad-bitrouter-smsm-test-04-trace \
	quad-bitrouter-smsm-test-05-trace quad-bitrouter-smsm-test-06-trace \
	quad-bitrouter-smsm-test-07-trace quad-bitrouter-smsm-test-08-trace \
	quad-bitrouter-smsm-test-09-trace quad-bitrouter-smsm-test-10-trace \
	quad-bitrouter-ssmm-test-01-trace quad-bitrouter-ssmm-test-02-trace \
	quad-bitrouter-ssmm-test-03-trace quad-bitrouter-ssmm-test-04-trace \
	quad-bitrouter-ssmm-test-05-trace quad-bitrouter-ssmm-test-06-trace \
	quad-bitrouter-ssmm-test-07-trace quad-bitrouter-ssmm-test-08-trace \
	quad-bitrouter-ssmm-test-09-trace quad-bitrouter-ssmm-test-10-trace

HACKT_CHPSIM_GUILE_TEST_SCRIPTS_FAILURES =

# custom hacchpimscm tests and post-processing
# keep .tmp file for reference
BITROUTER_HACCHPSIMSCM_TRUNCATE_TEST_SCRIPT =
if HAVE_LIBGUILE
# just truncate every test...
BITROUTER_HACCHPSIMSCM_TRUNCATE_TEST_SCRIPT += \
	mv $@ $@.tmp && \
	{ $(GREP) "^process" $@.tmp | $(TAIL) -n 1000 && \
	  $(SED) -n '/histogram:/,$$p' $@.tmp ;} > $@
endif
# this is appended to the end of HACCHPSIMSCM_TEST_SCRIPT


# NOTE: tracefiles produced as side effects are already detected and
# produce automatic dependencies in chpsimrc-out.autodepend

srcdir_EXTRA_DIST += \
	bitrouter-test-template.chpsimrc

###############################################################################

include $(top_srcdir)/test/Make.test-tail

# extend the default test script
if HAVE_LIBGUILE
HACCHPSIMSCM_TEST_SCRIPT += && \
	$(BITROUTER_HACCHPSIMSCM_TRUNCATE_TEST_SCRIPT)
endif

# clean up side-effect files from bitrouter tests
HACKT_GUILE_CLEAN_IGNORE_PATTERNS += \
	"*.hacchpsimscm-out.tmp"

# additional search path for scheme scripts
HACKT_GUILE_LOAD_PATH +=$(PATH_SEPARATOR)$(srcdir)/..

