# "test/parser/connect/Makefile.am"
#	$Id: Makefile.am,v 1.11 2005/09/04 21:15:42 fang Exp $

include $(top_srcdir)/test/Make.test-head

ARTC_TEST_FAILURES = \
	002 003 004 005 008 009 \
	016 \
	020 023 024 026 028 029 \
	032 033 034 035 036 037 038 039 \
	040 041 042 043 044 045 048 \
	050 052 053 054 055 056 059 \
	060 061 066 068 \
	070 071 072 073 074 075 076 077 079 \
	080 081 082 \
	090

ARTC_TEST_PASSES = \
	001 006 007 \
	010 011 012 013 014 015 017 018 019 \
	021 022 025 027 \
	030 031 031a 031b \
	046 047 049 \
	051 057 058 \
	062 063 064 065 067 069 \
	078 \
	083 084 085 086 086a 086b 087 088 089 \
	091 092 093 094 095 096 097 098 099 \
	100 101 102 103 103a 104 105 106 107 108 109 \
	110 111 112 113 114 114a 115 116 117 118 119 \
	120 121

ARTC_TEST_SUBJECTS = $(ARTC_TEST_FAILURES) $(ARTC_TEST_PASSES)

ARTOBJ_TEST_SUBJECTS = \
	$(ARTC_TEST_PASSES)

ARTOBJUNROLL_TEST_SUBJECTS = \
	$(ARTOBJ_TEST_SUBJECTS)

# note: known unroll rejects (should be excluded from create test)
# 091 096 097 098
# 105 108 110 115

# adding tests one-by-one, should be subset of ARTOBJUNROLL_TEST_SUBJECTS
HACKT_CREATE_TEST_SUBJECTS = \
	001 006 007 \
	010 011 012 013 014 015 017 018 019 \
	021 022 025 027 \
	030 031a 031b \
	046 047 049 \
	051 057 058 \
	062 063 064 065 067 069 \
	078 \
	083 084 085 088 089 \
	092 093 094 095 099 \
	100 102 103 104 106 107 109 \
	111 112 113 114 114a 118 119

# KNOWN BUG: 116 117: doesn't handle creation of bar
#	because no relaxed actual is passed in as a formal parameter.
# 117 passes unrolling, but fails during create's hierarchical type check, neat!
# KNOWN BUGS: 120 121 (creation phase, expect to fail, but doesn't)

# notes:
# the following tests depend channel port subinstances
# 086 087

# more output expectation files for when bison is used
BISON_OUTPUTS += 032 076 077

include $(top_srcdir)/test/Make.test-tail

