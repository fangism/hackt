# "test/parser/array/Makefile.am"
#	$Id: Makefile.am,v 1.9 2005/12/13 04:16:30 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = \
	    003 004 005 006 008 009 \
	011 012 014 017 018 019 \
	020 021 022 023 024 025 026 027 028 \
	030 031 032 033 034 035 036

HACKT_PARSE_TEST_PASSES = \
	001 002 007 \
	010 013 015 016 \
	029 \
	037 038 039 \
	040 041

# unfortunately is out of order... to bad $(sort ...) is non-posix
HACKT_PARSE_TEST_SUBJECTS = $(HACKT_PARSE_TEST_PASSES) $(HACKT_PARSE_TEST_FAILURES)

HACKT_OBJ_TEST_SUBJECTS = \
	$(HACKT_PARSE_TEST_PASSES)

HACKT_UNROLL_TEST_SUBJECTS = \
	$(HACKT_OBJ_TEST_SUBJECTS)

# The following fail during unrolling:
# 041
HACKT_CREATE_TEST_SUBJECTS = \
	001 002 007 \
	010 013 015 016 \
	029 \
	037 038 039 \
	040

include $(top_srcdir)/test/Make.test-tail

