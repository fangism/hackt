# "test/Makefile.am"
#	$Id: Makefile.am,v 1.10 2005/07/23 06:53:10 fang Exp $

include $(srcdir)/Make.test-head

# this directory first, to clean the test-report
SUBDIRS = . lexer parser

clean-test:
	@$(ECHO) "#!$(SHELL)" > $@
	@$(ECHO) "$(RM) $(TEST_REPORT)" >> $@
	@$(CHMOD) +x $@

# bogus test
TESTS = clean-test

DISTCLEANFILES += $(TESTS)

EXTRA_DIST += ChangeLog Make.test-head Make.test-tail \
	artc-expect.sh artobj-diff.sh \
	artobjunroll-expect.sh \
	state_enum_filter.awk address_filter.sed \
	POM-header-filter.awk \
	update-artcstderr.sh \
	update-unrollstderr.sh \
	spacestonewline.awk test.cvsignore \
	test-series.sh

# remove extraneous auto-generated files
dist-hook:
	-$(RM) -r `find $(distdir) -name subdir.not_a_test`
	-$(RM) -r `find $(distdir) -name clean-test`

# template test Makefile
include $(top_srcdir)/test/Make.test-tail

