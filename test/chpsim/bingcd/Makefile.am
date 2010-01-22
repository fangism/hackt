# "test/chpsim/bingcd/Makefile.am"
#	$Id: Makefile.am,v 1.3.2.1 2010/01/22 23:42:03 fang Exp $
# Test auto-Makefile for chpsim.  

include $(top_srcdir)/test/Make.test-head
# include $(top_srcdir)/installdirs.mk
# for examplesdir

# SUBDIRS = .

HACO_FLAGS += -I$(srcdir)/../testlib -I$(srcdir)/..

# list tests here only if they are also intended for create/alloc tests
HACKT_PARSE_TEST_PASSES =

# in this directory, no parse-fail tests
HACKT_CREATE_TEST_PASSES = $(HACKT_PARSE_TEST_PASSES)

HACKT_ALLOC_TEST_PASSES = $(HACKT_CREATE_TEST_PASSES)

# temporary for now:
HACKT_CHPSIM_ALLOC_TEST_SUBJECTS = $(HACKT_ALLOC_TEST_PASSES)

HACKT_CHPSIMRC_TEST_SUBJECTS = \
	gcd-env-channels \
	euclid-gcd \
	binary-gcd \
	euclid-gcd-seq-a \
	euclid-gcd-dec-a \
	euclidmod-gcd-seq-a \
	binary-gcd-seq-a \
	gcd-source-A-01 \
	gcd-source-B-01 \
	gcd-source-AB-01 \
	gcd-source-AB-02 \
	euclid-gcd-seq-a-01-01 \
	euclid-gcd-dec-a-01-01 \
	euclid-gcd-seq-a-01 euclid-gcd-seq-a-02 \
	euclidmod-gcd-seq-a-01 euclidmod-gcd-seq-a-02 \
	euclid-gcd-dec-a-01 euclid-gcd-dec-a-02 \
	binary-gcd-seq-a-01 binary-gcd-seq-a-02

# consider making .chpsim-event-dot/ps for visualizations

HACKT_CHPSIMRC_RANDOM_TEST_SCRIPTS_PASSES =

HACKT_CHPSIMRC_TEST_SCRIPTS_PASSES = \
	euclid-gcd-seq-a-01-01 \
	euclid-gcd-dec-a-01-01 \
	euclid-gcd-seq-a-01 euclid-gcd-seq-a-02 \
	euclid-gcd-dec-a-01 euclid-gcd-dec-a-02 \
	euclidmod-gcd-seq-a-01 euclidmod-gcd-seq-a-02 \
	binary-gcd-seq-a-01 binary-gcd-seq-a-02

# TODO: parallel-fifo test cases

HACKT_CHPSIMRC_TEST_SCRIPTS_FAILURES =

# not yet, three tests report checkpoint diffs due to queue-reorderings
HACKT_CHPSIM_CKPT_TEST_SCRIPTS_BASES = $(HACKT_CHPSIMRC_TEST_SCRIPTS_PASSES)

if RANDOM_TESTS
HACKT_CHPSIM_CKPT_TEST_SCRIPTS_BASES += \
	$(HACKT_CHPSIMRC_RANDOM_TEST_SCRIPTS_PASSES)
endif

HACKT_CHPSIM_GUILE_TEST_SCRIPTS_PASSES = \
	euclid-gcd-seq-a-01 euclid-gcd-seq-a-02 \
	euclid-gcd-dec-a-01 euclid-gcd-dec-a-02 \
	euclidmod-gcd-seq-a-01 euclidmod-gcd-seq-a-02 \
	binary-gcd-seq-a-01 binary-gcd-seq-a-02

HACKT_CHPSIM_GUILE_RANDOM_TEST_SCRIPTS_PASSES =

HACKT_CHPSIM_GUILE_TEST_SCRIPTS_FAILURES =


# NOTE: tracefiles produced as side effects are already detected and
# produce automatic dependencies in chpsimrc-out.autodepend

srcdir_EXTRA_DIST += \
	analyze-gcd.hacchpsimscm \
	gcd-cycle-template.chpsimrc \
	gcd-test-template.chpsimrc \
	gcd-dec-test-template.chpsimrc

###############################################################################

include $(top_srcdir)/test/Make.test-tail

# clean up side-effect files from bitrouter tests
# temporary:
# HACKT_GUILE_CLEAN_IGNORE_PATTERNS +=
HACKT_GUILE_CLEAN_IGNORE_PATTERNS = \
	"*.hacchpsimscm-out.tmp"

# additionsl search path for scheme scripts
GUILE_LOAD_FLAGS += -L $(srcdir)/..

