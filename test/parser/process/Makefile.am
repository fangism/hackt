# "test/parser/process/Makefile.am"

AUTOMAKE_OPTIONS = 1.9

SUFFIXES = .in .artctest .artobjtest

ARTC_TEST_SUBJECTS = \
	    001 002 003 004 005 006 007 008 009 \
	010 011 012 013 014 015 016 017 018 019 \
	020 021 022 023 024 025 026 027 028 029 \
	030 031 032 033 034 035 036 037 038 039 \
	040 041 042 043 044 045 046 047 048 049 \
	050

ARTOBJ_TEST_SUBJECTS = \
	020 022 024 027 028 029 \
	030 035 036 037 \
	040 041 042 043 049

TESTS = $(ARTC_TEST_SUBJECTS:=.artctest) \
	$(ARTOBJ_TEST_SUBJECTS:=.artobjtest)

# more output expectation files for when bison is used
BISON_OUTPUTS = 001 002 003 004 005 006 007 008 009 010 \
	011 012 013 014 015 016 017 018 019 021 023

# define empty, let template Makefile add to it
EXTRA_DIST =
CLEANFILES =
DISTCLEANFILES =

include $(top_srcdir)/test/Make.test

# dist-hook: dist-hook-extra

