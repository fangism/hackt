# "test/parser/channel/Makefile.am"
#	$Id: Makefile.am,v 1.7.28.1 2005/12/13 02:58:48 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = 000 003 004 006 007 \
	013 \
	022 023

HACKT_PARSE_TEST_PASSES = 001 002 005 008 009 010 \
	011 012 014 015 016 017 018 019 \
	020 021
# 024 pending...

HACKT_PARSE_TEST_SUBJECTS = $(HACKT_PARSE_TEST_FAILURES) $(HACKT_PARSE_TEST_PASSES)

# should be the subset of HACKT_PARSE_TEST_SUBJECTS that type-checks
HACKT_OBJ_TEST_SUBJECTS = \
	$(HACKT_PARSE_TEST_PASSES)

# should be the subset of HACKT_OBJ_TEST_SUBJECTS that passes unroll checks
HACKT_UNROLL_TEST_SUBJECTS = \
	$(HACKT_OBJ_TEST_SUBJECTS)

# more output expectation files for when bison is used
BISON_OUTPUTS += 000 003 004 006 007 \
	013

include $(top_srcdir)/test/Make.test-tail

