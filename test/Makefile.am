# "test/Makefile.am"
#	$Id: Makefile.am,v 1.16.10.1 2005/11/06 21:55:06 fang Exp $

include $(srcdir)/Make.test-head

# this directory first, to clean the test-report
SUBDIRS = . lexer parser benchmarks

clean-test:
	@$(ECHO) "#!$(SHELL)" > $@
	@$(ECHO) "$(RM) $(TEST_REPORT)" >> $@
	@$(CHMOD) +x $@

# bogus test
TESTS += clean-test

# configure-input scripts
PRECONFIGURED_TESTSCRIPTS = \
	artc-expect.sh.in \
	artobj-diff.sh.in \
	artobjunroll-expect.sh.in \
	hackt-create-expect.sh.in \
	hackt-alloc-expect.sh.in \
	hackt-cflat-expect.sh.in \
	test-series.sh.in

POSTCONFIGURED_TESTSCRIPTS = $(PRECONFIGURED_TESTSCRIPTS:.in=)
AUTO_IGNORE += $(POSTCONFIGURED_TESTSCRIPTS)

DISTCLEANFILES += $(TESTS) $(POSTCONFIGURED_TESTSCRIPTS)

EXTRA_DIST += ChangeLog README TODO \
	Make.test-head Make.test-tail \
	$(PRECONFIGURED_TESTSCRIPTS) \
	state_enum_filter.awk address_filter.sed \
	alloc_filter.sed \
	POM-header-filter.awk \
	POM-index-filter.awk \
	update-artcstderr.sh \
	update-artcstderr-bison.sh \
	update-unrollstderr.sh \
	update-createstderr.sh \
	update-allocstderr.sh \
	update-expectprs.sh \
	analyze-dump-diff.awk \
	spacestonewline.awk test.cvsignore

# remove extraneous auto-generated files
dist-hook:
	-$(RM) -r `find $(distdir) -name subdir.not_a_test`
	-$(RM) -r `find $(distdir) -name clean-test`

# template test Makefile
include $(top_srcdir)/test/Make.test-tail

