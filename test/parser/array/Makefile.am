# "test/parser/array/Makefile.am"
#	$Id: Makefile.am,v 1.18 2006/11/12 05:58:27 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = \
	    003 004 005 006 008 009 \
	011 012 014 \
	032 \
	042 045 048 049 \
	051 052 053 057

BISON_OUTPUTS += 042 049 057

# The following tests used to fail at compile-time when we were statically
# checking for ovelapping indices: 017-028, 030, 031, 033-036

HACKT_PARSE_TEST_PASSES = \
	001 002 007 \
	010 013 015 016 017 018 019 \
	020 021 022 023 024 025 026 027 028 029 \
	030 031 033 034 035 036 037 038 039 \
	040 041 043 044 046 047 \
	054 055 056 058 059 \
	060

HACKT_UNROLL_TEST_PASSES = \
	001 002 007 \
	010 013 015 016 \
	029 \
	037 038 039 \
	040 043 044 046 047 \
	054 056 \
	060

HACKT_UNROLL_TEST_FAILURES = \
	017 018 019 \
	020 021 022 023 024 025 026 027 028 \
	030 031 033 034 035 036 \
	041 \
	055 058 059

# The following fail during unrolling:
# 041
# Many other tests are just uninteresting beyond unrolling.
HACKT_CREATE_TEST_PASSES = \
	001 002 007 \
	010 013 015 016 \
	029 \
	037 038 039 \
	040 043 044 046 \
	054 \
	060

HACKT_CREATE_TEST_FAILURES =

HACKT_ALLOC_TEST_PASSES = $(HACKT_CREATE_TEST_PASSES)

HACKT_CFLAT_TEST_SUBJECTS = \
	043 046

# test-cases to process
# many of these are pending construction of higher dimension arrays
srcdir_EXTRA_DIST += \
	050.hac

# the following test cases should pass everything, 
# just need to update expected outputs at earliest convenience
srcdir_EXTRA_DIST += 061.hac

include $(top_srcdir)/test/Make.test-tail

