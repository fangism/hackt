# "test/parser/datatype/Makefile.am"

AUTOMAKE_OPTIONS = 1.9

SUFFIXES = .in .artctest .artobjtest

ARTC_TEST_SUBJECTS = \
	    001 002 003 004 005 006 007 008 009 \
	010 011 012 013 014 015 016 017 018 019 \
	020 021 022 023 024 025 026 027 028

ARTOBJ_TEST_SUBJECTS = \
	001 002 006 007 008 \
	013 015 016 \
	021 022 023 026 027 028

TESTS = $(ARTC_TEST_SUBJECTS:=.artctest) \
	$(ARTOBJ_TEST_SUBJECTS:=.artobjtest)

# more output expectation files for when bison is used
BISON_OUTPUTS = 017 018 019 020

# define empty, let template Makefile add to it
EXTRA_DIST =
CLEANFILES =
DISTCLEANFILES =

include $(top_srcdir)/test/Make.test

# dist-hook: dist-hook-extra

