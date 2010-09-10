# "test/parser/template/Makefile.am"
#	$Id: Makefile.am,v 1.32.4.1 2010/09/10 01:13:50 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = \
	    003 005 \
	010 013 014 019 \
	020 023 024 025 027 028 \
	042 047 048 \
	050 051 052 \
	060 065 066 069 \
	085 \
	103 104 \
	112 113 114 \
	125 126 \
	env_source_typedef-01a \
	env_source_typedef-01d \
	env_source_typedef-03 \
	act-006 act-009 act-010

# No longer statically checking dimensions, so the following tests pass parsing:
# 067

HACKT_PARSE_TEST_PASSES = \
	001 002 004 006 007 008 008a 009 \
	011 012 015 016 017 018 \
	021 022 026 029 \
	030 031 032 033 033a 034 035 036 037 038 038a 039 \
	040 041 043 044 045 046 049 \
	053 054 055 056 057 058 059 \
	061 062 063 064 064a 067 067a 067b 067c 067d 068 \
	070 071 072 073 073a 074 075 076 077 078 079 079b \
	080 081 082 083 084 086 087 088 089 \
	090 091 092 093 094 095 096 097 098 099 \
	100 101 102 105 106 107 108 109 \
	110 111 115 116 116b 117 117b 118 119 \
	120 121 122 123 124 127 128 129 \
	130 131 132 133 \
	env_source \
	env_source-01 env_source-01a env_source-01b \
	env_source-02 env_source-03 env_source-04 \
	env_source-05 env_source-06 env_source-07 env_source-08 \
	env_source-09 env_source-10 env_source-11 env_source-12 \
	env_source-13 env_source-13a env_source-14 env_source-14a \
	env_source-15 env_source-15a env_source-16 env_source-16a \
	env_source-17 env_source-18 \
	env_source-19 env_source-19a env_source-20 env_source-20a \
	env_source-A env_source-A-01 env_source-A-02 \
	env_source-A-03 \
	env_source-B env_source-B-01 env_source-B-02 \
	env_source-B-03 env_source-B-04 env_source-B-05 \
	env_source_pack env_source_pack-01 \
	env_source_pack-A env_source_pack-A-01 \
	env_source_pack-B env_source_pack-B-01 \
	env_source_ports env_source_ports-01 \
	env_source_ports-02 env_source_ports-03 \
	env_source_ports-04 env_source_ports-05 \
	env_source_ports-06 env_source_ports-07 \
	env_source_ports-08 env_source_ports-09 \
	env_source_ports-A env_source_ports-A-01 \
	env_source_ports-B env_source_ports-B-01 env_source_ports-B-02 \
	env_source_ports-C env_source_ports-C-01 \
	env_source_ports-C-02 env_source_ports-C-03 \
	env_source_recursive env_source_recursive-01 \
	env_source_recursive-A env_source_recursive-A-01 \
	env_source_ports_recursive env_source_ports_recursive-01 \
	env_source_typedef-01 env_source_typedef-01b env_source_typedef-01c \
	env_source_typedef-02 env_source_typedef-02a \
	env_source_typedef-04 env_source_typedef-04a env_source_typedef-04b \
	env_source_typedef-05 env_source_typedef-05a env_source_typedef-05b \
	env_source_typedef-06 env_source_typedef-06a env_source_typedef-06b \
	env_source_typedef-07 env_source_typedef-07a env_source_typedef-07b \
	env_source_typedef-08 env_source_typedef-08a env_source_typedef-08b \
	act-004 act-005 act-007 act-008 act-011
# pending: act-001 act-002 act-003

# KNOWN BUGS:
# should fail at unroll-time:
#	057 058 (not propagating relaxed formal dependence yet)

BISON_OUTPUTS += 027 028 042 050 103 \
	act-009

HACKT_CREATE_TEST_FAILURES = \
	008a \
	033a 034 037 038a 039 \
	041 \
	067 067a \
	071 072 073a 078 \
	080 082 083 084 \
	129

# omitted tests are just uninteresting
HACKT_CREATE_TEST_PASSES = \
	008 009 \
	011 012 \
	029 \
	038 \
	049 \
	059 \
	063 067d 068 \
	070 073 074 075 076 077 079 079b \
	081 086 088 \
	090 091 092 093 095 096 097 099 \
	100 101 102 106 107 \
	110 111 116 116b 117 117b 118 119 \
	120 122 123 124 127 128 \
	130 131 132 133 \
	env_source-01 env_source-01a \
	env_source-02 env_source-03 \
	env_source-05 env_source-06 env_source-08 \
	env_source-11 env_source-12 \
	env_source-13 env_source-13a env_source-14 env_source-14a \
	env_source-15 env_source-15a env_source-16 env_source-16a \
	env_source-18 \
	env_source-19 env_source-19a env_source-20 env_source-20a \
	env_source-A-02 env_source-A-03 \
	env_source-B-02 env_source-B-03 \
	env_source-B-04 env_source-B-05 \
	env_source_pack-01 \
	env_source_pack-A-01 \
	env_source_ports-01 env_source_ports-02 env_source_ports-05 \
	env_source_ports-08 \
	env_source_ports-B-01 env_source_ports-B-02 \
	env_source_ports-C-03 \
	env_source_recursive-01 \
	env_source_recursive-A-01 \
	env_source_ports_recursive-01 \
	env_source_typedef-01b env_source_typedef-01c \
	env_source_typedef-06b \
	env_source_typedef-07b \
	env_source_typedef-08b

HACKT_CREATE_TEST_FAILURES += \
	045 046 \
	061 062 064 064a 067 067a 067b 067c \
	087 089 \
	094 098 \
	108 109 \
	115 \
	121 \
	env_source-01b env_source-04 env_source-07 \
	env_source-09 env_source-10 \
	env_source-17 \
	env_source-A-01 \
	env_source-B-01 \
	env_source_pack-B-01 \
	env_source_ports-03 env_source_ports-04 \
	env_source_ports-06 env_source_ports-07 \
	env_source_ports-09 \
	env_source_ports-A-01 \
	env_source_ports-C-01 env_source_ports-C-02 \
	env_source_typedef-06a \
	env_source_typedef-07a \
	env_source_typedef-08a
# KNOWN BUGS: (ICE-on-invalid)
#	env_source_typedef-02a
#	env_source_typedef-04a env_source_typedef-04b
#	env_source_typedef-05a env_source_typedef-05b

HACKT_ALLOC_TEST_PASSES = \
	008 009 \
	011 012 \
	029 \
	038 \
	049 \
	063 067d 068 \
	070 073 074 075 076 077 079 079b \
	081 086 \
	090 091 092 093 095 096 097 099 \
	100 101 102 106 107 \
	110 111 116 116b 117 117b 118 119 \
	120 122 123 124 127 128 \
	130 131 132 133 \
	env_source-01 env_source-01a \
	env_source-02 env_source-03 \
	env_source-05 env_source-06 \
	env_source-08 env_source-11 env_source-12 \
	env_source-13 env_source-13a env_source-14 env_source-14a \
	env_source-15 env_source-15a env_source-16 env_source-16a \
	env_source-18 \
	env_source-19 env_source-19a env_source-20 env_source-20a \
	env_source-A-02 env_source-A-03 \
	env_source-B-02 env_source-B-03 env_source-B-04 env_source-B-05 \
	env_source_pack-01 \
	env_source_pack-A-01 \
	env_source_ports-01 env_source_ports-02 env_source_ports-05 \
	env_source_ports-08 \
	env_source_ports-C-03 \
	env_source_recursive-01 \
	env_source_recursive-A-01 \
	env_source_ports_recursive-01 \
	env_source_typedef-01b env_source_typedef-01c \
	env_source_typedef-06b \
	env_source_typedef-07b \
	env_source_typedef-08b

HACKT_ALLOC_TEST_FAILURES =

HACKT_CFLAT_TEST_SUBJECTS = \
	074 075 079 079b

include $(top_srcdir)/test/Make.test-tail

