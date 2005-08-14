# "test/Makefile.am"
#	$Id: Makefile.am,v 1.12.6.1 2005/08/14 20:09:04 fang Exp $

include $(srcdir)/Make.test-head

# this directory first, to clean the test-report
SUBDIRS = . lexer parser

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
	test-series.sh.in

POSTCONFIGURED_TESTSCRIPTS = $(PRECONFIGURED_TESTSCRIPTS:.in=)
AUTO_IGNORE += $(POSTCONFIGURED_TESTSCRIPTS)

DISTCLEANFILES += $(TESTS) $(POSTCONFIGURED_TESTSCRIPTS)

EXTRA_DIST += ChangeLog README \
	Make.test-head Make.test-tail \
	$(PRECONFIGURED_TESTSCRIPTS) \
	state_enum_filter.awk address_filter.sed \
	POM-header-filter.awk \
	POM-index-filter.awk \
	update-artcstderr.sh \
	update-unrollstderr.sh \
	update-createstderr.sh \
	analyze-dump-diff.awk \
	spacestonewline.awk test.cvsignore

# remove extraneous auto-generated files
dist-hook:
	-$(RM) -r `find $(distdir) -name subdir.not_a_test`
	-$(RM) -r `find $(distdir) -name clean-test`

# template test Makefile
include $(top_srcdir)/test/Make.test-tail

