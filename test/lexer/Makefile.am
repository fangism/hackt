# "test/lexer/Makefile.am"
#	$Id: Makefile.am,v 1.15 2006/11/15 00:09:23 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = \
	badid-000 badid-001 \
	hex-02 hex-03 \
	comment-000 comment-001 comment-002 \
	string-001 string-002 string-003 \
	import-000 import-001 import-002 import-003 import-005 \
	import-006 import-007 import-008 import-009 \
	import-010 import-011 import-014-c \
	import-016 import-017 import-018 import-019 \
	import-020 import-021 \
	line-number-01 line-number-02 line-number-03

HACKT_PARSE_TEST_PASSES = \
	comment-003 comment-004 comment-005 comment-006 \
	import-004 import-012 import-013 import-015-t \
	hex-01

HACKT_FLATTEN_TEST_PASSES = \
	badid-000 badid-001 \
	comment-003 comment-004 comment-005 comment-006 \
	comment-007 comment-008 \
	string-002 string-003 \
	import-000 import-004 import-012 import-013 \
	import-014-a import-014-b import-014-c \
	import-015-t import-022 \
	empty2 empty3

HACKT_FLATTEN_TEST_FAILURES = \
	comment-000 comment-001 comment-002 \
	comment-009 \
	string-001 \
	import-001 import-002 import-003 import-005 import-006 \
	import-007 import-008 import-009 import-010 import-011 \
	import-016 import-017 import-018 import-019 import-020 \
	import-021

# should be the subset of HACKT_PARSE_TEST_SUBJECTS that type-checks
# HACKT_OBJ_TEST_SUBJECTS = $(HACKT_PARSE_TEST_PASSES)

# should be the subset of HACKT_OBJ_TEST_SUBJECTS that passes unroll checks
# HACKT_UNROLL_TEST_SUBJECTS = $(HACKT_OBJ_TEST_SUBJECTS)

# additional sources included by other test cases
srcdir_EXTRA_DIST += empty.hac \
	import-014-a.hac import-014-b.hac \
	import-015-b.hac import-015-c.hac import-015-e.hac \
	import-015-g.hac import-015-p.hac

# more output expectation files for when bison is used
BISON_OUTPUTS += string-002 string-003 \
	import-014-c

include $(top_srcdir)/test/Make.test-tail

