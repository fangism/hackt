# "test/parser/namespace/Makefile.am"
#	$Id: Makefile.am,v 1.11.22.4 2006/11/11 06:28:34 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = \
	    006 \
	011 012 013 \
	021 023 024 026 028

# should be the subset of HACKT_PARSE_TEST_SUBJECTS that type-checks
HACKT_PARSE_TEST_PASSES = \
	    001 002 003 004 005 007 008 009 \
	010 014 015 016 017 018 019 \
	020 022 025 027 029

HACKT_UNROLL_TEST_PASSES = \
	$(HACKT_OBJ_TEST_SUBJECTS)

# the other tests have no instances and are uninteresting
HACKT_CREATE_TEST_PASSES = \
	003 004 005 027

HACKT_CREATE_TEST_FAILURES =

HACKT_ALLOC_TEST_PASSES = $(HACKT_CREATE_TEST_PASSES)

# more output expectation files for when bison is used
BISON_OUTPUTS += 026

include $(top_srcdir)/test/Make.test-tail

