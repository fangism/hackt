# "test/parser/connect/Makefile.am"
#	$Id: Makefile.am,v 1.20 2006/04/05 22:32:24 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = \
	002 003 004 005 008 009 \
	016 \
	026 028 029 \
	032 033 035 036 039 \
	040 041 042 043 044 045 048 \
	050 054 055 056 \
	061 \
	070 072 073 076 077 079 \
	080 081 082 \
	090

# The following tests now pass parsing since we've disabled static 
# compile-time checks:
# 020 023 024 034 037 038 052 053 059 060 066 068 071 074 075

HACKT_PARSE_TEST_PASSES = \
	001 006 006b 007 \
	010 011 012 013 014 015 017 018 019 \
	020 020a 021 022 023 023a 024 025 025b 027 027b \
	030 030b 031 031a 031b 034 037 038 \
	046 047 049 \
	051 052 053 057 058 059 \
	060 062 063 064 065 066 067 068 069 \
	071 074 075 078 \
	083 084 085 086 086a 086b 087 088 089 \
	091 092 099 \
	100 101 102 103 103a 104 105 106 107 108 109 \
	110 111 112 113 118 119 \
	122 123 124 125 126 127 128 129 \
	130

HACKT_PARSE_TEST_SUBJECTS = \
	$(HACKT_PARSE_TEST_FAILURES) \
	$(HACKT_PARSE_TEST_PASSES)

HACKT_OBJ_TEST_SUBJECTS = \
	$(HACKT_PARSE_TEST_PASSES)

HACKT_UNROLL_TEST_SUBJECTS = \
	$(HACKT_OBJ_TEST_SUBJECTS)

# the following tests are disabled until relaxed actuals are handled properly:
# 093 094 095
# 114 114a 115 116 117 
# 120 121

# 096, 097, 098, 115 -- should pass unroll, 
# and fail create once relaxed actuals are checked

# note: known unroll rejects (should be excluded from create test)
# 091 096 097 098
# 105 108 110 115

# adding tests one-by-one, should be subset of HACKT_UNROLL_TEST_SUBJECTS
HACKT_CREATE_TEST_SUBJECTS = \
	001 006 006b 007 \
	010 011 012 013 014 015 017 018 019 \
	021 022 025 025b 027 027b \
	030 030b 031a 031b \
	046 047 049 \
	051 057 058 \
	062 063 064 065 067 069 \
	078 \
	083 084 085 088 089 \
	092 099 \
	100 102 103 104 106 107 109 \
	111 112 113 118 119 \
	122 123 124 125 126 127 128 129 \
	130

# punted until unroll tests are restored: 114 114a
# KNOWN BUG: 116 117: doesn't handle creation of bar
#	because no relaxed actual is passed in as a formal parameter.
# 117 passes unrolling, but fails during create's hierarchical type check, neat!
# KNOWN BUGS: 120 121 (creation phase, expect to fail, but doesn't)

HACKT_ALLOC_TEST_SUBJECTS = \
	001 006 006b 007 \
	010 011 014 017 018 019 \
	021 022 025 025b 027 027b \
	030 030b \
	046 047 049 \
	051 057 \
	062 063 064 069 \
	078 \
	083 084 085 \
	111 112 113 118 119 \
	122 123 124 125 126 127 128 129 \
	130

# add 031a,b later... too tired to hand-verify
# add most CREATE tests to ALLOC tests later...

# notes:
# the following tests depend channel port subinstances
# 086 087

HACKT_CFLAT_TEST_SUBJECTS = \
	001 006b \
	025b 027b \
	030b \
	046 047 049 \
	051 057 \
	062 063 064 069 \
	078 \
	083 084 085 \
	111 112 113 118 119 \
	122 123 124 125 126 127 128 129 \
	130

# more output expectation files for when bison is used
BISON_OUTPUTS += 032 076 077

# pending test cases
EXTRA_DIST += 093.hac 094.hac 095.hac 096.hac 097.hac 098.hac \
	114.hac 114a.hac 115.hac 116.hac 117.hac 120.hac 121.hac

include $(top_srcdir)/test/Make.test-tail

