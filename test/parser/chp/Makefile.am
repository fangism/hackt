# "test/parser/chp/Makefile.am"
#	$Id: Makefile.am,v 1.4.28.1 2005/12/13 02:58:48 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = 001 003 004 006 007 008 009 \
	010 011 012 013 014 016 018 \
	023 \
	030 031 035 037 038 \
	046 047 048 \
	050 051 055 056 059 \
	062

HACKT_PARSE_TEST_PASSES = 000 002 005 \
	015 017 019 \
	020 021 022 024 025 026 027 028 029 \
	032 033 034 036 039 \
	040 041 042 043 044 045 049 \
	052 053 054 057 058 \
	060 061

HACKT_PARSE_TEST_SUBJECTS = $(HACKT_PARSE_TEST_FAILURES) $(HACKT_PARSE_TEST_PASSES)

# should be the subset of HACKT_PARSE_TEST_SUBJECTS that type-checks
HACKT_OBJ_TEST_SUBJECTS = \
	$(HACKT_PARSE_TEST_PASSES)

# should be the subset of HACKT_OBJ_TEST_SUBJECTS that passes unroll checks
HACKT_UNROLL_TEST_SUBJECTS = \
	$(HACKT_OBJ_TEST_SUBJECTS)

# more output expectation files for when bison is used
BISON_OUTPUTS += 001 003 004 \
	035 055 056 059 062

include $(top_srcdir)/test/Make.test-tail

