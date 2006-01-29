# "test/parser/namespace/Makefile.am"
#	$Id: Makefile.am,v 1.8.10.1 2006/01/29 04:42:38 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = \
	    006 \
	011 012 013 \
	021 023 024 026

# should be the subset of HACKT_PARSE_TEST_SUBJECTS that type-checks
HACKT_PARSE_TEST_PASSES = \
	    001 002 003 004 005 007 008 009 \
	010 014 015 016 017 018 019 \
	020 022 025

HACKT_PARSE_TEST_SUBJECTS = \
	$(HACKT_PARSE_TEST_FAILURES) $(HACKT_PARSE_TEST_PASSES)

HACKT_OBJ_TEST_SUBJECTS = \
	$(HACKT_PARSE_TEST_PASSES)

HACKT_UNROLL_TEST_SUBJECTS = \
	$(HACKT_OBJ_TEST_SUBJECTS)

# the other tests have no instances and are uninteresting
HACKT_CREATE_TEST_SUBJECTS = \
	003 004 005

HACKT_ALLOC_TEST_SUBJECTS = $(HACKT_CREATE_TEST_SUBJECTS)

# more output expectation files for when bison is used
BISON_OUTPUTS += 026

include $(top_srcdir)/test/Make.test-tail

