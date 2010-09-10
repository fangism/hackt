# "test/parser/typedef/Makefile.am"
#	$Id: Makefile.am,v 1.15.8.1 2010/09/10 01:14:06 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = \
	    001 006 007 008 009 \
	010 016 017 \
	021 022 023 027 028 029

HACKT_PARSE_TEST_PASSES = \
	002 003 004 005 \
	011 012 013 014 015 018 019 \
	020 024 025 026 \
	030 031

# temporary adds until bug fixed
EXTRA_DIST +=

# HACKT_UNROLL_TEST_PASSES = \
#	002 003 004 005 \
#	011 012 013 015 018 \
#	020

HACKT_CREATE_TEST_FAILURES = \
	014 019

HACKT_CREATE_TEST_PASSES = \
	002 003 004 005 \
	012 015 018 \
	020 024 025 026 \
	030 031

HACKT_ALLOC_TEST_PASSES = $(HACKT_CREATE_TEST_PASSES)

# no cflat/prs tests yet

# more output expectation files for when bison is used
BISON_OUTPUTS += 001

include $(top_srcdir)/test/Make.test-tail

