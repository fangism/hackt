# "test/parser/namespace/Makefile.am"
#	$Id: Makefile.am,v 1.17 2009/04/20 20:35:30 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = \
	    006 \
	011 012 013 \
	021 023 024 026 028 \
	030 031 032 034 035 036 \
	045 046 047 \
	067 \
	050-act 051-act 052-act 053-act 054-act \
	055-act 056-act 057-act 058-act 059-act \
	act-002 act-003 act-004

# should be the subset of HACKT_PARSE_TEST_SUBJECTS that type-checks
HACKT_PARSE_TEST_PASSES = \
	    001 002 003 004 005 005b 007 008 009 \
	010 014 015 016 017 018 019 \
	020 022 025 027 029 \
	033 037 038 039 \
	050 051 052 053 054 055 056 057 058 059 \
	060 061 062 063 065 \
	act-001
# pending cflat clarifications: 040 041 042 043 044

HACKT_PARSE_TEST_WARNINGS = \
	064 066

EXTRA_DIST += \
	040.hac 041.hac 042.hac 043.hac 044.hac 048.hac 049.hac

# the other tests have no instances and are uninteresting
HACKT_CREATE_TEST_PASSES = \
	003 004 005 005b 027 \
	act-001
# pending: 050 051 052 053 054 055 056 057
# should these be accepted? (compare with act)

HACKT_CREATE_TEST_FAILURES =

HACKT_ALLOC_TEST_PASSES = $(HACKT_CREATE_TEST_PASSES)

# more output expectation files for when bison is used
BISON_OUTPUTS += 026

include $(top_srcdir)/test/Make.test-tail

