# "test/Makefile.am"
#	$Id: Makefile.am,v 1.8.4.1 2005/07/09 23:13:20 fang Exp $

AUTOMAKE_OPTIONS = 1.9

# this directory first, to clean the test-report
SUBDIRS = . lexer parser

clean-test:
	@$(ECHO) "#!$(SHELL)" > $@
	@$(ECHO) "$(RM) $(TEST_REPORT)" >> $@
	@$(CHMOD) +x $@

# bogus test
TESTS = clean-test

DISTCLEANFILES = $(TESTS)

CLEANFILES = 

EXTRA_DIST = ChangeLog Make.test artc-expect.sh artobj-diff.sh \
	artobjunroll-expect.sh \
	state_enum_filter.awk address_filter.sed \
	POM-header-filter.awk \
	update-artcstderr.sh \
	update-unrollstderr.sh \
	spacestonewline.awk test.cvsignore \
	test-series.sh

# template test Makefile
include $(top_srcdir)/test/Make.test

# remove extraneous auto-generated files
dist-hook:
	-$(RM) -r `find $(distdir) -name subdir.not_a_test`
	-$(RM) -r `find $(distdir) -name clean-test`

