# "test/parser/process/Makefile.am"
#	$Id: Makefile.am,v 1.9 2005/08/08 16:51:53 fang Exp $

include $(top_srcdir)/test/Make.test-head

ARTC_TEST_FAILURES = \
	    001 002 003 004 005 006 007 008 009 \
	010 011 012 013 014 015 016 017 018 019 \
	021 023 025 026 \
	031 032 033 034 038 039 \
	041 044 045 046 047 048 \
	050 052 053

ARTC_TEST_PASSES = \
	020 022 024 027 028 029 \
	030 035 036 037 \
	040 042 043 049 \
	051 054 055 056 057 058 059 \
	060 061 062 063 064 065

ARTC_TEST_SUBJECTS = $(ARTC_TEST_FAILURES) $(ARTC_TEST_PASSES)

ARTOBJ_TEST_SUBJECTS = \
	$(ARTC_TEST_PASSES)

ARTOBJUNROLL_TEST_SUBJECTS = \
	$(ARTOBJ_TEST_SUBJECTS)

# the omitted unroll tests either have no instances or are just uninteresting
HACKT_CREATE_TEST_SUBJECTS = \
	024 \
	054 055 056 057 058 059 \
	060 061 062 063 064 065

# more output expectation files for when bison is used
BISON_OUTPUTS += 001 002 003 004 005 006 007 008 009 010 \
	011 012 013 014 015 016 017 018 019 021 023

include $(top_srcdir)/test/Make.test-tail

