# "test/parser/connect/Makefile.am"
#	$Id: Makefile.am,v 1.5 2005/01/28 19:59:14 fang Exp $

AUTOMAKE_OPTIONS = 1.9

SUFFIXES = .in .artctest .artobjtest .artobjunrolltest

ARTC_TEST_FAILURES = \
	    002 003 004 005 009 \
	016 \
	020 023 024 026 028 029 \
	032 033 034 035 036 037 038 039 \
	040 041 042 043 044 045 048 \
	050 052 053 054 055 056 059 \
	060 061 066 068 \
	070 071 072 073 074 075 076 077 079 \
	080 081 082

ARTC_TEST_PASSES = \
	001 006 007 008 \
	010 011 012 013 014 015 017 018 019 \
	021 022 025 027 \
	030 031 \
	046 047 049 \
	051 057 058 \
	062 063 064 065 067 069 \
	078 \
	083 084 085

ARTC_TEST_SUBJECTS = $(ARTC_TEST_FAILURES) $(ARTC_TEST_PASSES)

ARTOBJ_TEST_SUBJECTS = \
	$(ARTC_TEST_PASSES)

ARTOBJUNROLL_TEST_SUBJECTS =

TESTS = $(ARTC_TEST_SUBJECTS:=.artctest) \
	$(ARTOBJ_TEST_SUBJECTS:=.artobjtest) \
	$(ARTOBJUNROLL_TEST_SUBJECTS:=.artobjunrolltest)

# more output expectation files for when bison is used
BISON_OUTPUTS = 032 076 077

# define empty, let template Makefile add to it
EXTRA_DIST =
CLEANFILES =
DISTCLEANFILES =

include $(top_srcdir)/test/Make.test

# dist-hook: dist-hook-extra

