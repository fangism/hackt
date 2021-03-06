# "test/parser/connect/Makefile.am"
#	$Id: Makefile.am,v 1.43 2010/09/02 23:09:50 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACO_FLAGS += -f case-collision=ignore

HACKT_PARSE_TEST_FAILURES = \
	002 003 004 005 008 009 \
	016 \
	026 028 029 \
	032 033 035 036 039 \
	041 042 043 044 045 048 \
	050 054 055 056 \
	061 \
	070 072 073 076 077 079 \
	080 081 082 \
	090 \
	138 \
	162 162b \
	globals-01 globals-02 globals-03 globals-04 globals-05 globals-06 \
	globals-07 globals-08 globals-11 globals-12 \
	supply-01 supply-02 \
	act-006 act-007 act-009 act-010 act-011
# TODO: {bool,chan,data,proc}-dir-02.hac

# pending: member_nonmeta_instance_references: globals-09 globals-10
srcdir_EXTRA_DIST += globals-09.hac globals-10.hac

# The following tests now pass parsing since we've disabled static 
# compile-time checks:
# 020 023 024 034 037 038 052 053 059 060 066 068 071 074 075

HACKT_PARSE_TEST_PASSES = \
	001 006 006b 007 \
	010 011 012 013 014 015 017 018 019 \
	020 020a 021 022 023 023a 024 025 025b 025c 027 027b \
	030 030b 031 031a 031b 034 037 038 \
	040 046 047 049 \
	051 052 053 057 058 059 \
	060 062 063 064 065 066 067 068 069 069b \
	071 074 075 078 \
	083 084 085 088 089 \
	091 092 093 094 095 096 097 098 099 \
	100 101 102 103 103a 104 105 106 107 108 109 \
	110 111 112 113 114 114a 115 116 117 118 119 \
	120 120a 120b 121 122 123 124 125 126 127 127b 128 128b 129 129b 129c \
	130 131 132 133 134 135 136 137 139 \
	140 141 142 143 144 145 146 147 148 149 \
	150 151 152 153 154 155 155b 156 157 158 159 \
	160 161 163a 163b 163c 164 165 166 167 168 169 \
	170 171 172 173 \
	bool-dir-01 \
	data-dir-01 \
	chan-dir-01 \
	proc-dir-01 \
	chan-port-dir-01 \
	act-001 act-002 act-003 act-004 act-005 act-008 act-012

HACKT_CREATE_TEST_PASSES = \
	001 006 006b 007 \
	010 011 012 013 014 015 017 018 019 \
	021 022 025 025b 027 027b \
	030 030b 031a 031b \
	046 047 049 \
	051 057 058 \
	062 063 064 065 067 069 \
	078 \
	083 084 085 088 089 \
	093 094 095 099 \
	100 102 103 104 106 107 109 \
	111 112 113 114a 118 119 \
	122 123 124 125 126 127 128 129 129b 129c \
	130 131 132 133 134 136 137 139 \
	140 141 142 143 144 145 146 147 148 149 \
	150 151 152 153 154 155 156 157 158 159 \
	160 161 163a 163b 163c 165 167 169 \
	170 171 172 173 \
	act-001 act-002 act-003 act-005 act-012
# TODO: {bool,chan,data,proc}-dir-*

HACKT_CREATE_TEST_FAILURES = \
	020 020a 023 023a 024 \
	034 037 038 \
	040 \
	052 053 059 \
	060 066 068 \
	071 074 075 \
	091 092 096 097 098 \
	103a 105 108 \
	110 114 115 116 117 \
	120 120a 120b 121 \
	135 \
	164 166 168 \
	act-004 act-008

HACKT_ALLOC_TEST_PASSES = \
	001 006 006b 007 \
	010 011 014 017 018 019 \
	021 022 025 025b 027 027b \
	030 030b \
	046 047 049 \
	051 057 \
	062 063 064 069 \
	078 \
	083 084 085 \
	093 094 095 \
	111 112 113 114a 118 119 \
	122 123 124 125 126 127 128 129 \
	130 131 132 133 134 136 137 139 \
	140 141 142 143 144 145 146 147 148 149 \
	150 151 152 153 154 155 156 157 158 159 \
	160 161 163a 163b 163c 165 167 169 \
	170 171 172 \
	act-001 act-002 act-003 act-005 act-012

HACKT_ALLOC_TEST_FAILURES =

# add 031a,b later... too tired to hand-verify
# add most CREATE tests to ALLOC tests later...

# notes: (TODO)
# the following tests depend channel port subinstances
# 086 087
# the following tests require support for user-defined channels
# 086 086a 086b 087

srcdir_EXTRA_DIST += 086.hac 086a.hac 086b.hac 087.hac

HACKT_CFLAT_TEST_SUBJECTS = \
	001 006b \
	025b 025c 027b \
	030b \
	046 047 049 \
	051 057 \
	062 063 064 069 069b \
	078 \
	083 084 085 \
	111 112 113 118 119 \
	122 123 124 125 126 127 127b 128 128b 129 129b 129c \
	130 131 132 133 134 \
	140 141 142 143 144 145 146 147 148 149 \
	150 151 155 155b 156 157 158 159 \
	160 161 163a 163b 163c 

HACKT_CFLAT_VCDH_TEST_SUBJECTS = \
	001 006b \
	010 011 017 \
	025b 027b \
	030b 031 031b \
	046 047 049 \
	051 057 058 \
	062 063 064 065 067 \
	078 \
	083 084 085 \
	111 112 113 118 119 \
	122 123 124 125 126 127 128 129 129b \
	130 131 132 133 134 136 137 139 \
	140 142 144 146 \
	150 152 155 156 157 158 159 \
	160 161 163a 163b 163c 165 169 \
	act-005 act-012 \
	bool-dir-01 \
	chan-dir-01 \
	proc-dir-01

HACKT_GUILE_TEST_SCRIPTS_PASSES = \
	006-01 006-04 006-05 006-06 006-07 006-08 006-09 \
	006-10 006-11 006-12 006-13 \
	031a-01 031a-02 031a-03 031a-04

HACKT_GUILE_TEST_SCRIPTS_FAILURES = \
	006-02 006-03

# more output expectation files for when bison is used
BISON_OUTPUTS += 032 076 077 \
	supply-02 \
	act-007

srcdir_EXTRA_DIST += \
	bool-dir-02 \
	data-dir-02 \
	chan-dir-02 \
	proc-dir-02

include $(top_srcdir)/test/Make.test-tail

