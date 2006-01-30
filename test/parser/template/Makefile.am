# "test/parser/template/Makefile.am"
#	$Id: Makefile.am,v 1.12.10.1 2006/01/30 05:27:15 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = \
	    003 005 \
	010 013 014 019 \
	020 023 024 025 027 028 \
	042 047 048 \
	050 051 052 053 054 \
	060 065 066 067 069

HACKT_PARSE_TEST_PASSES = \
	001 002 004 006 007 008 008a 009 \
	011 012 015 016 017 018 \
	021 022 026 029 \
	030 031 032 033 033a 034 035 036 037 038 038a 039 \
	040 041 043 044 045 046 049 \
	055 056 057 058 059 \
	061 062 063 064 068 \
	070 071 072 073 073a

# KNOWN BUGS:
# should fail at unroll-time:
#	057 058 (not propagating relaxed formal dependence yet)

BISON_OUTPUTS += 027 028 042 047 048 050

HACKT_PARSE_TEST_SUBJECTS = \
	$(HACKT_PARSE_TEST_FAILURES) \
	$(HACKT_PARSE_TEST_PASSES)

HACKT_OBJ_TEST_SUBJECTS = \
	$(HACKT_PARSE_TEST_PASSES)

HACKT_UNROLL_TEST_SUBJECTS = \
	$(HACKT_OBJ_TEST_SUBJECTS)

# omitted tests are just uninteresting
HACKT_CREATE_TEST_SUBJECTS = \
	008 009 \
	011 012 \
	029 \
	038 \
	045 046 049 \
	059 \
	061 062 063 064 068 \
	070 073

HACKT_ALLOC_TEST_SUBJECTS = \
	008 009 \
	011 012 \
	029 \
	038 \
	049 \
	068 \
	070 073

# expected to fail create (OK):
# 045 046 061 062 064

# CRASH BUGS:
# alloc: 059 063 persistence! (discovered 20060129)

include $(top_srcdir)/test/Make.test-tail

