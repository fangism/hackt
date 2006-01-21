# "test/parser/typedef/Makefile.am"
#	$Id: Makefile.am,v 1.10.2.2 2006/01/21 10:09:26 fang Exp $

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

HACKT_PARSE_TEST_SUBJECTS = \
	$(HACKT_PARSE_TEST_FAILURES) \
	$(HACKT_PARSE_TEST_PASSES)

HACKT_OBJ_TEST_SUBJECTS = $(HACKT_PARSE_TEST_PASSES)

HACKT_UNROLL_TEST_SUBJECTS = $(HACKT_OBJ_TEST_SUBJECTS)

HACKT_CREATE_TEST_SUBJECTS = \
	002 003 004 005 \
	012 015 018 \
	020

HACKT_ALLOC_TEST_SUBJECTS = $(HACKT_CREATE_TEST_SUBJECTS)

# no cflat/prs tests yet

# more output expectation files for when bison is used
BISON_OUTPUTS += 001

include $(top_srcdir)/test/Make.test-tail

