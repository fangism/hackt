# "test/Makefile.am"

AUTOMAKE_OPTIONS = 1.9

SUBDIRS = . parser

clean-test:
	@$(ECHO) "#!$(SHELL)" > $@
	@$(ECHO) "$(RM) $(TEST_REPORT)" >> $@
	@$(CHMOD) +x $@

# bogus test
TESTS = clean-test

DISTCLEANFILES = $(TESTS)

CLEANFILES = 
EXTRA_DIST = Make.test artc-expect.sh state_enum_filter.awk

include $(top_srcdir)/test/Make.test

# remove extraneous auto-generated files
dist-hook:
	rm -rf `find $(distdir) -name subdir.not_a_test`
	rm -rf `find $(distdir) -name clean-test`

