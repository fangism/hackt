# "test/lexer/Makefile.am"
#	$Id: Makefile.am,v 1.4.2.2 2005/11/12 07:49:03 fang Exp $

include $(top_srcdir)/test/Make.test-head

ARTC_TEST_FAILURES = \
	badid-000 badid-001 \
	comment-000 comment-001 comment-002 \
	string-001 string-002 string-003 \
	import-000 import-001 import-002 import-003 import-005 \
	import-006 import-007 import-008 import-009 \
	import-010 import-011

ARTC_TEST_PASSES = \
	comment-003 comment-004 \
	import-004 import-012 import-013

ARTC_TEST_SUBJECTS = $(ARTC_TEST_FAILURES) $(ARTC_TEST_PASSES)

# should be the subset of ART_TEST_SUBJECTS that type-checks
# ARTOBJ_TEST_SUBJECTS = $(ARTC_TEST_PASSES)

# should be the subset of ARTOBJ_TEST_SUBJECTS that passes unroll checks
# ARTOBJUNROLL_TEST_SUBJECTS = $(ARTOBJ_TEST_SUBJECTS)

EXTRA_DIST += empty.in

# more output expectation files for when bison is used
BISON_OUTPUTS += string-002 string-003

include $(top_srcdir)/test/Make.test-tail

