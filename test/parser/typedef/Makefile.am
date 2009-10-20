# "test/parser/typedef/Makefile.am"
#	$Id: Makefile.am,v 1.15 2009/10/20 22:01:11 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = \
	    001 006 007 008 009 \
	010 016 017 \
	021 022 023

HACKT_PARSE_TEST_PASSES = \
	002 003 004 005 \
	011 012 013 014 015 018 019 \
	020 024

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
	020 024

HACKT_ALLOC_TEST_PASSES = $(HACKT_CREATE_TEST_PASSES)

# no cflat/prs tests yet

# more output expectation files for when bison is used
BISON_OUTPUTS += 001

include $(top_srcdir)/test/Make.test-tail

