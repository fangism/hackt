# "test/parser/chp/Makefile.am"
#	$Id: Makefile.am,v 1.6.2.8 2006/06/23 07:55:51 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = \
	001 003 004 006 007 008 009 \
	010 011 012 013 014 016 018 \
	023 \
	030 031 035 037 038 \
	046 047 048 \
	050 051 055 056 059 \
	062 067 068 \
	071 072 073 076

HACKT_PARSE_TEST_PASSES = \
	000 002 005 \
	015 017 019 \
	020 021 022 024 025 026 027 028 029 \
	032 033 034 036 039 \
	040 041 042 043 044 045 049 \
	052 053 054 057 058 \
	060 061 063 064 065 066 069 \
	070 074 075 077 078 079 \
	080 081 082 083

HACKT_PARSE_TEST_SUBJECTS = \
	$(HACKT_PARSE_TEST_FAILURES) \
	$(HACKT_PARSE_TEST_PASSES)

# should be the subset of HACKT_PARSE_TEST_SUBJECTS that type-checks
HACKT_OBJ_TEST_SUBJECTS = \
	$(HACKT_PARSE_TEST_PASSES)

# should be the subset of HACKT_OBJ_TEST_SUBJECTS that passes unroll checks
HACKT_UNROLL_TEST_SUBJECTS = \
	$(HACKT_OBJ_TEST_SUBJECTS)

HACKT_CREATE_TEST_SUBJECTS = \
	064 \
	074 075 077 078 079 \
	080 081 082 083

# TODO: alloc phase
# several tests are not ready because we haven't defined
# footprint frames for built-in channel types yet.  
# They result in an assertion failure on the frame pointer.  
# HACKT_ALLOC_TEST_SUBJECTS = \
#	$(HACKT_CREATE_TEST_SUBJECTS)

# more output expectation files for when bison is used
BISON_OUTPUTS += 001 003 004 \
	035 055 056 059 062

# pending test cases
EXTRA_DIST +=

include $(top_srcdir)/test/Make.test-tail

