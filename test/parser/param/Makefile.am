# "test/parser/param/Makefile.am"
#	$Id: Makefile.am,v 1.25 2011/03/12 02:51:01 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = \
	    005 006 007 009 \
	010 011 012 015 \
	023 024 025 026 027 029 \
	030 031 032 034 035 \
	051 052 053 054 056 \
	070 \
	090 091 092 099 \
	100 101 \
	112 \
	122 128 129

# BISON_OUTPUTS +=

# 016, 018 were moved to PASSES since we disabled static
# dimension checking.  
# the type-checked subset of the above
HACKT_PARSE_TEST_PASSES = \
	001 002 003 004 008 \
	013 014 016 017 018 019 \
	020 021 022 028 \
	033 036 037 038 039 \
	040 041 042 043 044 045 046 047 048 \
	050 055 057 058 059 \
	060 061 062 063 064 065 066 067 068 069 \
	071 072 073 074 075 076 077 078 079 \
	080 081 082 083 084 085 086 087 088 089 \
	093 094 095 096 097 098 \
	102 103 104 105 106 107 108 109 \
	110 111 113 114 115 116 117 118 119 \
	120 121 123 124 125 126 127 \
	130 131 \
	string-fun-01 \
	math-fun-01 math-fun-02

# subset of HACKT_OBJ_TEST_SUBJECTS
# TODO: sync this list up with CREATE_PASSES
# HACKT_UNROLL_TEST_PASSES = \
#	110 111 116 117 118 119 \
#	120 121

HACKT_CREATE_TEST_FAILURES = \
	013 016 018 \
	020 021 022 028 \
	033 \
	048 \
	072 075 077 \
	084 085 \
	095 \
	107 \
	113 114 115 \
	125

# TODO: preal test cases
# do some serious arithmetic
# perform some comparisons

# KNOWN BUG:
# 049 -- reference to out-of-scope value (global) is yet unsupported.

HACKT_CREATE_TEST_PASSES = \
	001 002 003 004 008 \
	014 017 019 \
	036 037 038 039 \
	040 041 042 043 044 045 046 047 \
	050 055 057 058 059 \
	060 061 062 063 064 065 066 067 068 069 \
	071 073 074 076 078 079 \
	080 081 082 083 086 087 088 089 \
	093 094 096 097 098 \
	102 103 104 105 106 108 109 \
	110 111 116 \
	121 123 124 126 127 \
	130 131 \
	string-fun-01 \
	math-fun-01 math-fun-02


HACKT_ALLOC_TEST_PASSES = $(HACKT_CREATE_TEST_PASSES)

include $(top_srcdir)/test/Make.test-tail

