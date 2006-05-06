# "test/Makefile.am"
#	$Id: Makefile.am,v 1.28 2006/05/06 04:19:01 fang Exp $

include $(srcdir)/Make.test-head

# this directory first, to clean the test-report
SUBDIRS = . lexer parser prsim benchmarks

# bogus test
TESTS +=

# configure-input scripts
PRECONFIGURED_TESTSCRIPTS = \
	parse-test-expect.sh.in \
	hackt-obj-diff.sh.in \
	hackt-unroll-expect.sh.in \
	hackt-create-expect.sh.in \
	check-create-consistency.sh.in \
	check-alloc-consistency.sh.in \
	hackt-alloc-expect.sh.in \
	hackt-cflat-expect.sh.in \
	hackt-cflat-seu-expect.sh.in \
	hackt-cflat-sprs-expect.sh.in \
	hackt-prsim-expr-alloc-expect.sh.in \
	hackt-prsimrc-expect.sh.in \
	hackt-prsim-ckpt.sh.in \
	hackt-prsim-opt.sh.in \
	test-series.sh.in

POSTCONFIGURED_TESTSCRIPTS = $(PRECONFIGURED_TESTSCRIPTS:.in=)
AUTO_IGNORE += $(POSTCONFIGURED_TESTSCRIPTS)

# These are useful maintainer script that we don't bother configure-generating
NONCONFIGURED_UPDATESCRIPTS = \
	update-compilestderr.sh \
	update-compilestderr-bison.sh \
	update-unrollstderr.sh \
	update-createstderr.sh \
	update-allocstderr.sh \
	update-expectprs.sh \
	update-expectprsseu.sh \
	update-expectprsimexpr.sh \
	update-expectsprs.sh \
	update-expect-prsimrc.sh

DISTCLEANFILES += $(TESTS) $(POSTCONFIGURED_TESTSCRIPTS)

EXTRA_DIST += ChangeLog README TODO \
	Make.test-head Make.test-tail \
	$(PRECONFIGURED_TESTSCRIPTS) \
	$(NONCONFIGURE_UPDATESCRIPTS) \
	state_enum_filter.awk address_filter.sed \
	alloc_filter.sed \
	POM-header-filter.awk \
	POM-index-filter.awk \
	vpath_file_filter.awk \
	hackt-parse-expect-filter.awk \
	analyze-dump-diff.awk \
	prsimrc-filter.awk \
	prsimrc-random-time-filter.awk \
	spacestonewline.awk test.cvsignore

all-local: $(POSTCONFIGURED_TESTSCRIPTS)

# template test Makefile
include $(top_srcdir)/test/Make.test-tail

