# "test/chpsim/Makefile.am"
#	$Id: Makefile.am,v 1.25.6.1 2007/09/21 19:51:17 fang Exp $
# Test auto-Makefile for chpsim.  

include $(top_srcdir)/test/Make.test-head
include $(top_srcdir)/installdirs.mk
# for examplesdir

SUBDIRS = testlib .

HACO_FLAGS += -I$(srcdir)/testlib
CHPSIM_LIB_PATH = ../../lib

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
	bool-or-test-a bool-or-test-b bool-or-test-c \
	bool-xor-test-a \
	channel-probe-a channel-probe-b channel-probe-c \
	send-fail-a \
	receive-fail-a \
	gcd-nested-a gcd-nested-b \
	logical-func-a logical-func-b \
	relational-func-a

# in this directory, no parse-fail tests
HACKT_CREATE_TEST_PASSES = $(HACKT_PARSE_TEST_PASSES)

HACKT_ALLOC_TEST_PASSES = $(HACKT_CREATE_TEST_PASSES)

# temporary for now:
HACKT_CHPSIM_ALLOC_TEST_SUBJECTS = $(HACKT_ALLOC_TEST_PASSES)

# NOTE: tests related to the CHP standard library should be prefixed 'lib-'
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
	channel-self-parallel-c \
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
	bool-and-test-b \
	token-ring-a \
	bubble-ring-a \
	deadlock-buf-a \
	deadlock-ring-a \
	timed-fifo-a timed-fifo-b timed-fifo-c timed-fifo-d timed-fifo-e \
	timed-token-ring-a timed-token-ring-b timed-token-ring-c \
	timed-bubble-ring-a timed-bubble-ring-b timed-bubble-ring-c \
	fib-test-a fib-test-b fib-test-c \
	bool-split-alternator-a \
	bool-merge-alternator-a \
	bool-parallel-fifo-a \
	source-sink-peekbuf-a source-sink-peekbuf-b \
	mux-test-flat \
	source-sink-void-a \
	missing_func-a missing_func-b missing_func-c missing_func-d \
	greeting_func-a \
	gcd-nested-a gcd-nested-b \
	logical-func-a logical-func-b \
	relational-func-a \
	lib-assert-test-a \
	lib-zcond-test-a \
	lib-select-test-a lib-select-test-b lib-select-test-c \
	lib-echo-test-a lib-echo-test-b lib-echo-test-c lib-echo-test-d \
	lib-print-test-a \
	lib-zscan-test-a lib-zscan-test-b lib-zscan-test-c \
	lib-bscan-test-a \
	lib-sscan-test-a lib-sscan-test-b \
	lib-fprint-test-a \
	lib-fflush-test-a \
	lib-fclose-test-a lib-fclose-test-b \
	lib-fappend-test-a \
	lib-fzscan-test-a lib-fzscan-test-b \
	lib-fzscan_loop-test-a lib-fzscan_loop-test-b \
	lib-sprint-test-a \
	lib-strtoz-test-a lib-strtoz-test-b \
	lib-strtoz-test-c lib-strtoz-test-d \
	lib-strtob-test-a lib-strtob-test-b lib-strtob-test-c \
	lib-bzscan-test-a \
	lib-xzscan-test-a \
	lib-fxzscan-test-a \
	lib-fxzscan_loop-test-a

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
	channel-self-parallel-a-01 channel-self-parallel-a-02 \
	channel-self-parallel-b-01 channel-self-parallel-b-02 \
	channel-self-parallel-c-01 channel-self-parallel-c-02 \
	do-while-countdown-01 do-while-countdown-02 \
	do-while-countdown-03-watch \
	do-while-countdown-04-break \
	do-while-countdown-05-break \
	do-while-countdown-06-watch \
	do-while-countdown-07-break \
	do-while-countdown-08 \
	countdown-successor-01 countdown-successor-02 \
	countdown-loop-a-01 countdown-loop-b-01 \
	countdown-loop-a-02-trace \
	countdown-loop-a-03 \
	countdown-sequence-a-01 \
	countdown-nested-a-01 countdown-nested-a-02 \
	countdown-nested-a-03-trace countdown-nested-a-04 \
	countdown-nested-b-01 countdown-nested-b-02 \
	countdown-nested-b-03-trace countdown-nested-b-04 \
	countdown-nested-c-01 countdown-nested-c-02 \
	countdown-nested-c-03-trace countdown-nested-c-04 \
	countdown-nested-d-01 countdown-nested-d-02 \
	countdown-nested-d-03-trace countdown-nested-d-04 \
	countdown-domino-a-01 countdown-domino-a-02 \
	countdown-domino-b-01 \
	countdown-domino-b-02-cause countdown-domino-b-03-cause \
	countdown-domino-b-04 \
	wait-basic-a-01 wait-basic-a-02 wait-basic-a-03 \
	wait-conjunction-a-01 wait-conjunction-a-02 wait-conjunction-a-03 \
	wait-conjunction-b-01 wait-conjunction-b-02 wait-conjunction-b-03 \
	wait-conjunction-c-01 wait-conjunction-c-02 wait-conjunction-c-03 \
	wait-indexed-guard-a-01 wait-indexed-guard-a-02 \
	wait-indexed-guard-a-03 \
	select-skip-a-01 select-skip-a-02 \
	select-skip-b-01 \
	select-sequence-a-01 select-sequence-a-02-cause \
	select-sequence-a-03 select-sequence-a-04-cause \
	select-sequence-a-05 \
	select-parallel-a-01 select-parallel-a-02 \
	select-parallel-a-03 select-parallel-a-04 \
	select-parallel-a-05 \
	select-loop-a-01 select-loop-a-02 \
	select-loop-b-01 select-loop-b-02 \
	arbitrate-skip-a-01 arbitrate-skip-a-02 \
	arbitrate-skip-b-01 \
	arbitrate-loop-a-01 arbitrate-loop-a-02 \
	arbitrate-loop-a-03 arbitrate-loop-a-04 \
	select-nested-loop-a-01 select-nested-loop-a-02 \
	select-nested-loop-a-03-trace \
	select-nested-loop-a-04 \
	select-nested-loop-b-01 select-nested-loop-b-02 \
	select-nested-loop-b-03-trace \
	select-nested-loop-b-04 \
	select-block-a-01 select-block-a-02 select-block-a-03 \
	bool-split-a-01 bool-split-a-02 \
	bool-split-select-a-01 \
	bool-split-select-a-02-trace \
	bool-split-select-a-03 \
	bool-lookup-a-01 bool-lookup-a-02 \
	channel-sync-desync-a-01 channel-sync-desync-a-02 \
	channel-sync-desync-a-03-trace \
	channel-sync-desync-a-04-trace \
	channel-sync-desync-a-05-trace \
	channel-sync-desync-a-06 \
	bool-memory-read-a-01 bool-memory-read-a-02 \
	bool-memory-write-a-01 bool-memory-write-a-02 \
	bool-and-test-a-01 \
	bool-and-test-a-02-break \
	bool-and-test-a-03 bool-and-test-a-04 \
	bool-and-test-b-01 bool-and-test-b-02-trace \
	bool-or-test-a-01 \
	bool-or-test-b-01 \
	bool-or-test-c-01 \
	bool-xor-test-a-01 \
	buffer-advance-to \
	channel-probe-a-01 channel-probe-a-02 channel-probe-a-03 \
	channel-probe-b-01 \
	channel-probe-c-01 channel-probe-c-02 channel-probe-c-03 \
	delayed-source-a-01 delayed-source-a-02-trace \
	delayed-sink-a-01 delayed-sink-a-02-trace \
	token-ring-a-01 token-ring-a-02 \
	bubble-ring-a-01 bubble-ring-a-02 \
	deadlock-buf-a-01 \
	deadlock-ring-a-01 \
	timed-fifo-a-01 timed-fifo-b-01 timed-fifo-c-01 \
	timed-fifo-d-01 timed-fifo-e-01 \
	timed-token-ring-a-01 timed-token-ring-b-01 timed-token-ring-c-01 \
	timed-token-ring-a-02 \
	timed-token-ring-a-03 \
	timed-bubble-ring-a-01 timed-bubble-ring-b-01 timed-bubble-ring-c-01 \
	timed-bubble-ring-a-02 \
	fib-test-a-01 fib-test-b-01 fib-test-c-01 \
	fib-test-a-02 fib-test-b-02 fib-test-c-02 \
	bool-split-alternator-a-01 bool-split-alternator-a-02 \
	bool-merge-alternator-a-01 bool-merge-alternator-a-02 \
	bool-parallel-fifo-a-01 \
	source-sink-peekbuf-a-01 source-sink-peekbuf-a-02 \
	source-sink-peekbuf-a-03 \
	source-sink-peekbuf-b-01 source-sink-peekbuf-b-02 \
	source-sink-peekbuf-b-03 \
	mux-test-flat-01 \
	source-sink-void-a-01 source-sink-void-a-02 \
	gcd-func-a-01 gcd-func-b-01 gcd-func-c-01 \
	gcd-func-a-05 gcd-func-a-06 gcd-func-a-08 gcd-func-a-09 \
	gcd-func-a-10 gcd-func-a-12 gcd-func-a-13 gcd-func-a-14 \
	greeting_func-a-01 greeting_func-a-02 \
	greeting_func-a-03 greeting_func-a-04 \
	gcd-nested-a-01 gcd-nested-a-03 \
	gcd-nested-b-01 \
	relational-func-a-01 \
	lib-zcond-test-a-01 \
	lib-select-test-a-01 \
	lib-echo-test-a-01 lib-echo-test-b-01 \
	lib-echo-test-c-01 lib-echo-test-d-01 \
	lib-print-test-a-01 \
	lib-fprint-test-a-01 \
	lib-fflush-test-a-01 \
	lib-fclose-test-a-01 lib-fclose-test-b-01 \
	lib-fappend-test-a-01 \
	lib-fzscan-test-a-01 \
	lib-fzscan_loop-test-a-01 \
	lib-fzscan_loop-test-b-01 \
	lib-sprint-test-a-01 \
	lib-strtoz-test-a-01 lib-strtoz-test-b-01 \
	lib-strtob-test-a-01 \
	lib-fxzscan-test-a-01 \
	lib-fxzscan_loop-test-a-01
# accept invalid? -- excess arguments dropped
#	gcd-func-a-05

# TODO: parallel-fifo test cases

HACKT_CHPSIMRC_TEST_SCRIPTS_FAILURES = \
	select-fail-a-01 \
	bool-split-b-01 \
	bool-split-select-b-01 \
	bool-lookup-b-01 \
	bool-memory-read-b-01 \
	bool-memory-write-b-01 \
	send-fail-a-01 \
	receive-fail-a-01 \
	missing_func-a-01 missing_func-b-01 \
	missing_func-c-01 missing_func-d-01 \
	gcd-func-a-03 gcd-func-a-04 gcd-func-a-07 gcd-func-a-11 \
	gcd-nested-a-02 gcd-nested-b-02 \
	logical-func-a-01 logical-func-a-02 \
	logical-func-b-01 \
	relational-func-a-02 \
	lib-assert-test-a-01 \
	lib-select-test-c-01 \
	lib-zscan-test-a-01 lib-zscan-test-a-02 lib-zscan-test-a-03 \
	lib-zscan-test-b-01 \
	lib-zscan-test-c-01 \
	lib-bscan-test-a-01 lib-bscan-test-a-02 \
	lib-sscan-test-a-01 lib-sscan-test-b-01 \
	lib-fzscan-test-b-01 \
	lib-strtoz-test-c-01 lib-strtoz-test-d-01 \
	lib-strtob-test-b-01 lib-strtob-test-c-01 \
	lib-bzscan-test-a-01 lib-bzscan-test-a-02 lib-bzscan-test-a-03 \
	lib-xzscan-test-a-01 lib-xzscan-test-a-02
# FIXME: ISEs on invalid, diagnostic needed
#	gcd-func-a-02
#	lib-assert-test-b-01 (assert-fail needs diagnostic)

# quarantined test cases:
srcdir_EXTRA_DIST += \
	gcd-func-a-02.chpsimrc \
	lib-assert-test-b-01.chpsimrc

# not yet, three tests report checkpoint diffs due to queue-reorderings
HACKT_CHPSIM_CKPT_TEST_SCRIPTS_BASES = $(HACKT_CHPSIMRC_TEST_SCRIPTS_PASSES)

HACKT_CHPSIM_GUILE_TEST_SCRIPTS_PASSES = \
	channel-sync-desync-a-01 channel-sync-desync-a-02 \
	channel-sync-desync-a-03 channel-sync-desync-a-04 \
	channel-sync-desync-a-05 channel-sync-desync-a-06 \
	channel-sync-desync-a-07 channel-sync-desync-a-08 \
	channel-sync-desync-a-09 channel-sync-desync-a-10 \
	channel-sync-desync-a-11 channel-sync-desync-a-12 \
	channel-sync-desync-a-13 \
	bool-split-select-a-01 bool-split-select-a-02 \
	bool-split-select-a-03 bool-split-select-a-04 \
	bool-split-select-a-05 bool-split-select-a-06 \
	bool-split-select-a-07 bool-split-select-a-08 \
	bool-split-select-a-09 \
	channel-sync-desync-a-14 channel-sync-desync-a-15 \
	channel-sync-desync-a-16 channel-sync-desync-a-17 \
	channel-sync-desync-a-18 channel-sync-desync-a-19 \
	channel-sync-desync-a-20 channel-sync-desync-a-21 \
	channel-sync-desync-a-22 channel-sync-desync-a-23 \
	channel-sync-desync-a-24 channel-sync-desync-a-25 \
	channel-sync-desync-a-26 channel-sync-desync-a-27 \
	channel-sync-desync-a-28 channel-sync-desync-a-29 \
	channel-sync-desync-a-30 channel-sync-desync-a-31 \
	channel-sync-desync-a-32 channel-sync-desync-a-33 \
	channel-sync-desync-a-34 channel-sync-desync-a-35 \
	channel-sync-desync-a-36 \
	select-nested-loop-a-01 select-nested-loop-a-02 \
	select-nested-loop-a-03 select-nested-loop-a-04 \
	select-nested-loop-a-05 select-nested-loop-a-06 \
	select-nested-loop-a-07 select-nested-loop-a-08 \
	select-nested-loop-b-08 \
	countdown-loop-a-01 countdown-loop-a-02 \
	countdown-loop-a-03 countdown-loop-a-04 \
	countdown-nested-a-01 countdown-nested-a-02 \
	countdown-nested-b-01 countdown-nested-b-02 \
	countdown-nested-c-01 countdown-nested-c-02 \
	countdown-nested-d-01 countdown-nested-d-02 \
	timed-fifo-a-01 timed-fifo-b-01 timed-fifo-c-01 \
	timed-fifo-d-01 timed-fifo-e-01 \
	timed-token-ring-a-01 timed-token-ring-b-01 timed-token-ring-c-01 \
	timed-token-ring-a-02 \
	timed-bubble-ring-a-01 timed-bubble-ring-b-01 timed-bubble-ring-c-01 \
	fib-test-a-01 fib-test-b-01 fib-test-c-01 \
	bool-split-alternator-a-01 \
	bool-merge-alternator-a-01 \
	source-sink-peekbuf-a-01 source-sink-peekbuf-a-02 \
	source-sink-peekbuf-b-01 source-sink-peekbuf-b-02


HACKT_CHPSIM_GUILE_TEST_SCRIPTS_FAILURES =

# additional sources not already mentioned
# srcdir_EXTRA_DIST +=

KEEP_CHPSIM_TRACE_FILES = \
	channel-sync-desync-a-04.chpsimtrace \
	channel-sync-desync-a-05.chpsimtrace \
	select-nested-loop-a-03.chpsimtrace \
	select-nested-loop-b-03.chpsimtrace \
	bool-split-select-a-02.chpsimtrace \
	countdown-loop-a-02.chpsimtrace \
	countdown-nested-a-03.chpsimtrace \
	countdown-nested-b-03.chpsimtrace \
	countdown-nested-c-03.chpsimtrace \
	countdown-nested-d-03.chpsimtrace \
	token-ring-a-01.chpsimtrace \
	bubble-ring-a-01.chpsimtrace \
	timed-fifo-a-01.chpsimtrace \
	timed-fifo-b-01.chpsimtrace \
	timed-fifo-c-01.chpsimtrace \
	timed-fifo-d-01.chpsimtrace \
	timed-fifo-e-01.chpsimtrace \
	timed-token-ring-a-01.chpsimtrace \
	timed-token-ring-a-02.chpsimtrace \
	timed-token-ring-b-01.chpsimtrace \
	timed-token-ring-c-01.chpsimtrace \
	timed-bubble-ring-a-01.chpsimtrace \
	timed-bubble-ring-b-01.chpsimtrace \
	timed-bubble-ring-c-01.chpsimtrace \
	fib-test-a-01.chpsimtrace \
	fib-test-b-01.chpsimtrace \
	fib-test-c-01.chpsimtrace \
	bool-split-alternator-a-01.chpsimtrace \
	bool-merge-alternator-a-01.chpsimtrace \
	source-sink-peekbuf-a-02.chpsimtrace \
	source-sink-peekbuf-b-02.chpsimtrace

# NOTE: tracefiles produced as side effects are already detected and
# produce automatic dependencies in chpsimrc-out.autodepend

# this should be redundant with chpsimrc-out dependence...
$(KEEP_CHPSIM_TRACE_FILES): $(HACKT_CHPSIM_EXE)

# generate input file in build directory for scan tests
fzscan-01.txt: Makefile
	{ $(ECHO) "5" ; \
	$(ECHO) "4" ; \
	$(ECHO) "3" ; \
	$(ECHO) "2" ; \
	$(ECHO) "1" ; \
	$(ECHO) "0" ;} > $@

fxzscan-01.txt: Makefile
	{ $(ECHO) "0001" ; \
	$(ECHO) "C0De" ; \
	$(ECHO) "DEAD" ; \
	$(ECHO) "beef" ; \
	$(ECHO) "f00d" ; \
	$(ECHO) "d000dE" ;} > $@

CLEANFILES += fzscan-01.txt fxzscan-01.txt

srcdir_EXTRA_DIST += \
	random-timing-test-short.chpsimrc \
	random-timing-test-medium.chpsimrc \
	random-timing-test-long.chpsimrc \
	random-timing-test-series.chpsimrc \
	event-classification.hacchpsimscm \
	event-types.hacchpsimscm \
	basic-trace-analysis.hacchpsimscm \
	trace-reference-cycle.hacchpsimscm

AM_CPPFLAGS = -I $(top_srcdir)/src -DBUILT_HACKT -UHAVE_CONFIG_H

# noinst_LTLIBRARIES =
# only build these when depended upon
EXTRA_LTLIBRARIES = \
	gcd.la \
	bad_gcd1.la bad_gcd2.la bad_gcd3.la bad_gcd4.la \
	libgcdstatic.la \
	gcdwrap.la \
	greeting.la

libs: $(LTLIBRARIES) $(EXTRA_LTLIBRARIES)

clean-libs:
	$(RM) $(LTLIBRARIES) $(EXTRA_LTLIBRARIES)

.PHONY: clean-libs libs

clean-local: clean-libs

# force shared library building:
noinst_MODULE_FLAGS = -module -rpath /nowhere
# bogus -rpath flag causes shared convenience libraries to be created
# Thanks to a tip from Ralf Wildenhues

gcd_la_SOURCES = gcd.cc
gcd_la_LDFLAGS = $(noinst_MODULE_FLAGS)

# various versions that should cause run-time failures
bad_gcd1_la_SOURCES = bad_gcd1.cc
bad_gcd1_la_LDFLAGS = $(noinst_MODULE_FLAGS)

bad_gcd2_la_SOURCES = bad_gcd2.cc
bad_gcd2_la_LDFLAGS = $(noinst_MODULE_FLAGS)

bad_gcd3_la_SOURCES = bad_gcd3.cc
bad_gcd3_la_LDFLAGS = $(noinst_MODULE_FLAGS)

bad_gcd4_la_SOURCES = bad_gcd4.cc
bad_gcd4_la_LDFLAGS = $(noinst_MODULE_FLAGS)

greeting_la_SOURCES = greeting.cc
greeting_la_LDFLAGS = $(noinst_MODULE_FLAGS)

# static library only
libgcdstatic_la_SOURCES = gcdstatic.cc gcdstatic.h
# libgcdstatic_la_LDFLAGS = -static
# -static flag is unnecessary, convenience library is static by default

# the wrapper creates a dynamic library from a static library
gcdwrap_la_SOURCES = gcdwrap.cc
gcdwrap_la_LIBADD = libgcdstatic.la
gcdwrap_la_LDFLAGS = $(noinst_MODULE_FLAGS)


###############################################################################
############################ install checking #################################
# we don't want to list this as a SUBDIR because it includes only a 
# minimalist Makefile without most standard automake targets.
# We need to manage the subdirectory here.  
installcheckdir = installtest

all-local: $(installcheckdir)/Makefile

$(installcheckdir)/Makefile: \
		$(installcheckdir)/Makefile.in $(top_builddir)/config.status
	$(RECONFIGURE)

$(installcheckdir)/Makefile: $(top_builddir)/installcheck.mk \
	$(top_srcdir)/installdirs.mk


# library install check test
libinstallcheckdir = libinstalltest
all-local: $(libinstallcheckdir)/Makefile

$(libinstallcheckdir)/Makefile: \
		$(libinstallcheckdir)/Makefile.in $(top_builddir)/config.status
	$(RECONFIGURE)

$(libinstallcheckdir)/Makefile: $(top_builddir)/installcheck.mk \
	$(top_srcdir)/installdirs.mk

# explicitly prepend path to force subshell to find installed executables first
INSTALL_PATH_OVERRIDE = $(bindir)$(PATH_SEPARATOR)"$$PATH"

INSTALLTEST_DIRS = $(installcheckdir) $(libinstallcheckdir)

if INSTALL_TESTS
installcheck-local: $(installcheckdir)/Makefile $(libinstallcheckdir)/Makefile
	PATH=$(INSTALL_PATH_OVERRIDE) ; \
	export PATH ; \
	$(ECHO) "installcheck-local" ; \
	list='$(INSTALLTEST_DIRS)' ; \
	for d in $$list ; do \
	  $(ECHO) "installcheck: make-ing all and clean in $$d ..." && \
	  if (cd $$d && \
	    $(MAKE) $(AM_MAKEFLAGS) && \
	    $(MAKE) $(AM_MAKEFLAGS) clean \
	  ) ; then : ; else exit 1 ; fi \
	done
endif

# clean-local: clean-installcheck
## won't work: dependent makefile is uninstalled before cleaning
# clean-installcheck:
#	(cd $(installcheckdir) && $(MAKE) $(AM_MAKEFLAGS) clean)

chpsimfuncdir = $(examplesdir)/chpsim-function
# MAINTAINENCE: keep chpsimfuncdir definition consistent with 
#	$(installcheckdir)/Makefile.in

DISTCLEANFILES += $(installcheckdir)/Makefile

dist_chpsimfunc_DATA = \
	$(installcheckdir)/README \
	$(installcheckdir)/hello.cc \
	$(installcheckdir)/hello-test.hac \
	$(installcheckdir)/hello-test.chpsimrc \
	$(installcheckdir)/Makefile.in

# library install test files, reusing existing build check test files
chpsimlibdir = $(examplesdir)/chpsim-stdlib
DISTCLEANFILES += $(libinstallcheckdir)/Makefile

chpsimlib_DATA = \
	$(libinstallcheckdir)/Makefile.in \
	lib-zcond-flat.hac \
	lib-zcond-test-a-01.chpsimrc

# flatten source so it doesn't depend on testlib imports after installed
lib-zcond-flat.hac: lib-zcond-test-a.hac
	$(HACKT_FLATTEN_EXE) $(HACO_FLAGS) $< > $@

CLEANFILES += lib-zcond-flat.hac

############################ end install checking #############################
###############################################################################

include $(top_srcdir)/test/Make.test-tail

