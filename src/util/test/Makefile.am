# "src/util/test/Makefile.am"

AUTOMAKE_OPTIONS = 1.9 subdir-objects

SUFFIXES = .test
TEST_EXPECT_SH = $(srcdir)/test-expect.sh

include $(top_srcdir)/Make.stddef

.cc.test:
	@$(ECHO) "#!$(SHELL)" > $@
	@$(ECHO) "# \"$@\"" >> $@
	@$(ECHO) $(TEST_EXPECT_SH) $* $(srcdir) >> $@
	@$(CHMOD) +x $@

noinst_PROGRAMS = multidimensional_qmap_test \
	multikey_qmap_test

multidimensional_qmap_test_SOURCES = multidimensional_qmap_test.cc

multikey_qmap_test_SOURCES = multikey_qmap_test.cc

# the util/ source directory
AM_CPPFLAGS = -I$(srcdir)/..

MORE_CFLAGS = -pipe -W -Wall -Wno-unused -Werror

AM_CFLAGS = $(MORE_CFLAGS)
AM_CXXFLAGS = $(MORE_CFLAGS)

EXTRA_DIST = test-expect.sh $(noinst_PROGRAMS:=.stderr)

CLEANFILES = \
	$(noinst_PROGRAMS:=.test) \
	$(noinst_PROGRAMS:=.diff) \
	$(noinst_PROGRAMS:=.out)

TESTS = $(noinst_PROGRAMS:=.test)

clean-local:
	-$(RM) *.core core.*
	-$(RM) *.diff *.test *.out

