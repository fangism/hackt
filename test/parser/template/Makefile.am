# "test/parser/template/Makefile.am"
#	$Id: Makefile.am,v 1.25.4.15 2007/07/16 20:50:12 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = \
	    003 005 \
	010 013 014 019 \
	020 023 024 025 027 028 \
	042 047 048 \
	050 051 052 053 054 \
	060 065 066 069 \
	085 \
	103 104 105 \
	env_source_typedef-03

# No longer statically checking dimensions, so the following tests pass parsing:
# 067

HACKT_PARSE_TEST_PASSES = \
	001 002 004 006 007 008 008a 009 \
	011 012 015 016 017 018 \
	021 022 026 029 \
	030 031 032 033 033a 034 035 036 037 038 038a 039 \
	040 041 043 044 045 046 049 \
	055 056 057 058 059 \
	061 062 063 064 064a 067 067a 067b 067c 067d 068 \
	070 071 072 073 073a 074 075 076 077 078 079 \
	080 081 082 083 084 086 087 088 089 \
	090 091 092 093 094 095 096 097 098 099 \
	100 101 102 106 107 108 109 \
	env_source \
	env_source-01 env_source-01a env_source-01b \
	env_source-02 env_source-03 env_source-04 \
	env_source-05 env_source-06 env_source-07 env_source-08 \
	env_source-09 env_source-10 env_source-11 env_source-12 \
	env_source-13 env_source-13a env_source-14 env_source-14a \
	env_source-15 env_source-15a env_source-16 env_source-16a \
	env_source-A env_source-A-01 env_source-A-02 \
	env_source-A-03 \
	env_source-B env_source-B-01 env_source-B-02 \
	env_source-B-03 env_source-B-04 env_source-B-05 \
	env_source_pack env_source_pack-01 \
	env_source_pack-A env_source_pack-A-01 \
	env_source_pack-B env_source_pack-B-01 \
	env_source_ports env_source_ports-01 \
	env_source_ports-A env_source_ports-A-01 \
	env_source_recursive env_source_recursive-01 \
	env_source_recursive-A env_source_recursive-A-01 \
	env_source_ports_recursive env_source_ports_recursive-01 \
	env_source_typedef-01 env_source_typedef-02 \
	env_source_typedef-04 env_source_typedef-05

# KNOWN BUGS:
# should fail at unroll-time:
#	057 058 (not propagating relaxed formal dependence yet)

BISON_OUTPUTS += 027 028 042 050
# 103

HACKT_UNROLL_TEST_PASSES = \
	001 002 004 006 007 008 009 \
	011 012 015 016 017 018 \
	021 022 026 029 \
	030 031 032 033 035 036 038 \
	040 043 044 045 046 049 \
	055 056 057 058 059 \
	061 062 063 064 064a 068 \
	070 073 074 075 076 077 079 \
	081 086 087 088 089

HACKT_UNROLL_TEST_FAILURES = \
	008a \
	033a 034 037 038a 039 \
	041 \
	067 067a \
	071 072 073a 078 \
	080 082 083 084

# omitted tests are just uninteresting
HACKT_CREATE_TEST_PASSES = \
	008 009 \
	011 012 \
	029 \
	038 \
	049 \
	059 \
	063 067d 068 \
	070 073 074 075 076 077 079 \
	081 086 088 \
	090 091 092 093 095 096 097 099 \
	100 101 102 106 107 \
	env_source-01 env_source-01a \
	env_source-02 env_source-03 \
	env_source-05 env_source-06 env_source-08 \
	env_source-11 env_source-12 \
	env_source-13 env_source-13a env_source-14 env_source-14a \
	env_source-15 env_source-15a env_source-16 env_source-16a \
	env_source_pack-01 \
	env_source_pack-A-01 \
	env_source_ports-01 \
	env_source-A-02 env_source-A-03 \
	env_source-B-02 env_source-B-03 \
	env_source-B-04 env_source-B-05 \
	env_source_recursive-01 \
	env_source_recursive-A-01 \
	env_source_ports_recursive-01


HACKT_CREATE_TEST_FAILURES = \
	045 046 \
	061 062 064 064a 067 067a 067b 067c \
	087 089 \
	094 098 \
	108 109 \
	env_source-01b env_source-04 env_source-07 \
	env_source-09 env_source-10 \
	env_source-A-01 \
	env_source-B-01 \
	env_source_pack-B-01 \
	env_source_ports-A-01

if USE_UNROLL_PHASE
else
HACKT_CREATE_TEST_FAILURES += $(HACKT_UNROLL_TEST_FAILURES)
endif

HACKT_ALLOC_TEST_PASSES = \
	008 009 \
	011 012 \
	029 \
	038 \
	049 \
	063 067d 068 \
	070 073 074 075 076 077 079 \
	081 086 \
	090 091 092 093 095 096 097 099 \
	100 101 102 106 107 \
	env_source-01 env_source-01a \
	env_source-02 env_source-03 \
	env_source-05 env_source-06 \
	env_source-08 env_source-11 env_source-12 \
	env_source-13 env_source-13a env_source-14 env_source-14a \
	env_source-15 env_source-15a env_source-16 env_source-16a \
	env_source-A-02 env_source-A-03 \
	env_source-B-02 env_source-B-03 env_source-B-04 env_source-B-05 \
	env_source_pack-01 \
	env_source_pack-A-01 \
	env_source_ports-01 \
	env_source_recursive-01 \
	env_source_recursive-A-01 \
	env_source_ports_recursive-01

HACKT_ALLOC_TEST_FAILURES =

HACKT_CFLAT_TEST_SUBJECTS = \
	074 075 079

include $(top_srcdir)/test/Make.test-tail

