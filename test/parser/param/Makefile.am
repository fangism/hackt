# "test/parser/param/Makefile.am"
#	$Id: Makefile.am,v 1.9.18.2 2006/02/09 00:35:19 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = \
	    005 006 007 009 \
	010 011 012 013 015 016 018 \
	023 024 025 026 027 029 \
	030 031 032 034 035 \
	051 052 053 054 056 057

# BISON_OUTPUTS +=

# the type-checked subset of the above
HACKT_PARSE_TEST_PASSES = \
	001 002 003 004 008 \
	014 017 019 \
	020 021 022 028 \
	033 036 037 038 039 \
	040 041 042 043 044 045 046 047 048 \
	050 055

HACKT_PARSE_TEST_SUBJECTS = \
	$(HACKT_PARSE_TEST_FAILURES) \
	$(HACKT_PARSE_TEST_PASSES)

HACKT_OBJ_TEST_SUBJECTS = \
	$(HACKT_PARSE_TEST_PASSES)

# subset of HACKT_OBJ_TEST_SUBJECTS
HACKT_UNROLL_TEST_SUBJECTS = \
	001 002 003 004 008 \
	014 017 019 \
	036 037 038 039 \
	040 041 042 043 044 045 046 047 \
	050 055

# TODO: preal test cases
# do some serious arithmetic
# perform some comparisons

# KNOWN BUG:
# 049 -- reference to out-of-scope value (global) is yet unsupported.

HACKT_CREATE_TEST_SUBJECTS = $(HACKT_UNROLL_TEST_SUBJECTS)

HACKT_ALLOC_TEST_SUBJECTS = $(HACKT_CREATE_TEST_SUBJECTS)

include $(top_srcdir)/test/Make.test-tail

