# "test/parser/typedef/Makefile.am"
#	$Id: Makefile.am,v 1.12 2006/11/12 05:59:21 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = \
	    001 006 007 008 009 \
	010 016 017

HACKT_PARSE_TEST_PASSES = \
	002 003 004 005 \
	011 012 013 014 015 018 019 \
	020

# temporary adds until bug fixed
EXTRA_DIST +=

HACKT_UNROLL_TEST_PASSES = \
	002 003 004 005 \
	011 012 013 015 018 \
	020

HACKT_UNROLL_TEST_FAILURES = \
	014 019

HACKT_CREATE_TEST_PASSES = \
	002 003 004 005 \
	012 015 018 \
	020

HACKT_CREATE_TEST_FAILURES =

HACKT_ALLOC_TEST_PASSES = $(HACKT_CREATE_TEST_PASSES)

# no cflat/prs tests yet

# more output expectation files for when bison is used
BISON_OUTPUTS += 001

include $(top_srcdir)/test/Make.test-tail

