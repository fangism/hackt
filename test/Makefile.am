# "test/Makefile.am"
#	$Id: Makefile.am,v 1.4 2004/11/02 07:52:26 fang Exp $

AUTOMAKE_OPTIONS = 1.9

# this directory first, to clean the test-report
SUBDIRS = . parser

clean-test:
	@$(ECHO) "#!$(SHELL)" > $@
	@$(ECHO) "$(RM) $(TEST_REPORT)" >> $@
	@$(CHMOD) +x $@

# bogus test
TESTS = clean-test

DISTCLEANFILES = $(TESTS)

CLEANFILES = 

EXTRA_DIST = Make.test artc-expect.sh artobj-diff.sh \
	state_enum_filter.awk address_filter.sed \
	test.cvsignore

# template test Makefile
include $(top_srcdir)/test/Make.test

# remove extraneous auto-generated files
dist-hook:
	-$(RM) -r `find $(distdir) -name subdir.not_a_test`
	-$(RM) -r `find $(distdir) -name clean-test`

