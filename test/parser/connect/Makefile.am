# "test/parser/connect/Makefile.am"
#	$Id: Makefile.am,v 1.4 2004/11/02 07:52:29 fang Exp $

AUTOMAKE_OPTIONS = 1.9

SUFFIXES = .in .artctest .artobjtest

ARTC_TEST_SUBJECTS = \
	    001 002 003 004 005 006 007 008 009 \
	010 011 012 013 014 015 016 017 018 019 \
	020 021 022 023 024 025 026 027 028 029 \
	030 031 032 033 034 035 036 037 038 039 \
	040 041 042 043 044 045 046 047 048 049 \
	050 051 052 053 054 055 056 057 058 059 \
	060 061 062 063 064 065 066 067 068 069 \
	070 071 072 073 074 075 076 077 078 079 \
	080 081 082 083 084 085

ARTOBJ_TEST_SUBJECTS = \
	001 006 007 008 \
	010 011 012 013 014 015 017 018 019 \
	021 022 025 027 \
	030 031 \
	046 047 049 \
	051 057 058 \
	062 063 064 065 \
	078 \
	083 084 085

TESTS = $(ARTC_TEST_SUBJECTS:=.artctest) \
	$(ARTOBJ_TEST_SUBJECTS:=.artobjtest)

# more output expectation files for when bison is used
BISON_OUTPUTS = 032 076 077

# define empty, let template Makefile add to it
EXTRA_DIST =
CLEANFILES =
DISTCLEANFILES =

include $(top_srcdir)/test/Make.test

# dist-hook: dist-hook-extra

