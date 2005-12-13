# "test/parser/datatype/Makefile.am"
#	$Id: Makefile.am,v 1.10.28.1 2005/12/13 02:58:50 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = \
	    003 004 005 009 \
	010 011 012 014 015 017 018 019 \
	020 024 025

HACKT_PARSE_TEST_PASSES = \
	001 002 006 007 008 \
	013 016 \
	021 022 023 026 027 028 029 \
	030

HACKT_PARSE_TEST_SUBJECTS = $(HACKT_PARSE_TEST_FAILURES) $(HACKT_PARSE_TEST_PASSES)

HACKT_OBJ_TEST_SUBJECTS = \
	$(HACKT_PARSE_TEST_PASSES)

HACKT_UNROLL_TEST_SUBJECTS = \
	$(HACKT_OBJ_TEST_SUBJECTS)

# omitted tests are uninteresting
HACKT_CREATE_TEST_SUBJECTS = \
	001 002 006 007 008 \
	023 026

# more output expectation files for when bison is used
BISON_OUTPUTS += 015 017 018 019 020

include $(top_srcdir)/test/Make.test-tail

