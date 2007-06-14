# "test/Makefile.am"
#	$Id: Makefile.am,v 1.43.2.1 2007/06/14 07:15:19 fang Exp $

include $(top_srcdir)/test/Make.test-head

# this directory first, to clean the test-report
SUBDIRS = . lexer parser prsim chpsim guile lib benchmarks quarantine

# bogus test
TESTS +=

OBSOLETE_TESTSCRIPTS = \
	parse-test-expect.sh.in \
	hackt-obj-diff.sh.in \
	hackt-unroll-expect.sh.in \
	hackt-create-expect.sh.in \
	check-create-consistency.sh.in \
	check-alloc-consistency.sh.in \
	hackt-alloc-expect.sh.in \
	hackt-cflat-expect.sh.in \
	hackt-cflat-lvs-expect.sh.in \
	hackt-cflat-seu-expect.sh.in \
	hackt-cflat-sprs-expect.sh.in \
	hackt-prsimrc-expect.sh.in \
	hackt-prsim-ckpt.sh.in \
	hackt-prsim-opt.sh.in \
	hackt-prsim-expr-alloc-expect.sh.in \
	hackt-prs-dot-test.sh.in

# configure-input scripts
PRECONFIGURED_TESTSCRIPTS = \
	test-series.sh.in

POSTCONFIGURED_TESTSCRIPTS = $(PRECONFIGURED_TESTSCRIPTS:.in=)
check_SCRIPTS = $(POSTCONFIGURED_TESTSCRIPTS)
AUTO_IGNORE += $(POSTCONFIGURED_TESTSCRIPTS)

# already automatic
# $(POSTCONFIGURE_TESTSCRIPTS): $(top_builddir)/config.status

# These are useful maintainer script that we don't bother configure-generating
NONCONFIGURED_UPDATESCRIPTS = \
	update-compilestderr.sh \
	update-compilestderr-bison.sh \
	update-flatten-expect.sh \
	update-unrollstderr.sh \
	update-createstderr.sh \
	update-allocstderr.sh \
	update-expectprs.sh \
	update-expectprsseu.sh \
	update-expectprsimexpr.sh \
	update-expectsprs.sh \
	update-expect-prsimrc.sh \
	update-expect-chpsimalloc.sh \
	update-expect-chpsimrc.sh \
	update-expect-hacscm.sh \
	update-expect-hacchpsimscm.sh

dist_noinst_SCRIPTS = \
	$(NONCONFIGURED_UPDATESCRIPTS) \
	state_enum_filter.awk \
	address_filter.sed \
	alloc_filter.sed \
	POM-header-filter.awk.in \
	POM-index-filter.awk \
	POM-size-filter.awk \
	vpath_file_filter.awk \
	hackt-parse-expect-filter.awk \
	hackt-flatten-expect-filter.awk \
	analyze-dump-diff.awk \
	prsimrc-filter.awk \
	prsimrc-random-time-filter.awk \
	chpsim-binary-size-filter.awk \
	spacestonewline.awk \
	prsimckpt-template.prsimrc \
	prsimckpt-load-template.prsimrc \
	prsimopt-template.prsimrc \
	chpsimckpt-template.chpsimrc \
	chpsimckpt-load-template.chpsimrc

# NOTE: spacestonewline.awk is no longer needed for Makefiles
# as it defined inline in the top-level Make.stddef

AUTO_IGNORE += POM-header-filter.awk

all-local: POM-header-filter.awk

DISTCLEANFILES += $(TESTS) $(POSTCONFIGURED_TESTSCRIPTS)

EXTRA_DIST += ChangeLog README TODO test.cvsignore \
	Make.test-head Make.test-tail

# automatically distributed, via configure sources:
#	$(PRECONFIGURED_TESTSCRIPTS)

# dist-hook:
# 	bogus='$(BOGUS_TARGETS)'; \
# 	distdir=$(distdir) ; \
# 	for b in $$bogus ; do \
# 		$(RM) -r `$(FIND) $$distdir -name "$$b"` ; \
# 	done


# template test Makefile
include $(top_srcdir)/test/Make.test-tail

