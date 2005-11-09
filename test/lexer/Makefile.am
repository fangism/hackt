# "test/lexer/Makefile.am"
#	$Id: Makefile.am,v 1.3.24.1 2005/11/09 03:27:39 fang Exp $

include $(top_srcdir)/test/Make.test-head

ARTC_TEST_FAILURES = \
	badid-000 badid-001 \
	comment-000 comment-001 comment-002 \
	string-001 string-002 string-003
#	import-000 import-001 import-002
# KNOWN BUG: in line number reporting in the import cases, reason known

ARTC_TEST_PASSES = \
	comment-003 comment-004

ARTC_TEST_SUBJECTS = $(ARTC_TEST_FAILURES) $(ARTC_TEST_PASSES)

# should be the subset of ART_TEST_SUBJECTS that type-checks
# ARTOBJ_TEST_SUBJECTS = $(ARTC_TEST_PASSES)

# should be the subset of ARTOBJ_TEST_SUBJECTS that passes unroll checks
# ARTOBJUNROLL_TEST_SUBJECTS = $(ARTOBJ_TEST_SUBJECTS)

# more output expectation files for when bison is used
BISON_OUTPUTS += string-002 string-003

include $(top_srcdir)/test/Make.test-tail

