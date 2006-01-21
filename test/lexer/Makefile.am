# "test/lexer/Makefile.am"
#	$Id: Makefile.am,v 1.6.2.1 2006/01/21 21:56:23 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = \
	badid-000 badid-001 \
	comment-000 comment-001 comment-002 \
	string-001 string-002 string-003 \
	import-000 import-001 import-002 import-003 import-005 \
	import-006 import-007 import-008 import-009 \
	import-010 import-011 import-014-c

HACKT_PARSE_TEST_PASSES = \
	comment-003 comment-004 \
	import-004 import-012 import-013

HACKT_PARSE_TEST_SUBJECTS = \
	$(HACKT_PARSE_TEST_FAILURES) \
	$(HACKT_PARSE_TEST_PASSES)

# should be the subset of HACKT_PARSE_TEST_SUBJECTS that type-checks
# HACKT_OBJ_TEST_SUBJECTS = $(HACKT_PARSE_TEST_PASSES)

# should be the subset of HACKT_OBJ_TEST_SUBJECTS that passes unroll checks
# HACKT_UNROLL_TEST_SUBJECTS = $(HACKT_OBJ_TEST_SUBJECTS)

EXTRA_DIST += empty.hac \
	import-014-a.hac import-014-b.hac

# more output expectation files for when bison is used
BISON_OUTPUTS += string-002 string-003 \
	import-014-c

include $(top_srcdir)/test/Make.test-tail

