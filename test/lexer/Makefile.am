# "test/lexer/Makefile.am"
#	$Id: Makefile.am,v 1.27 2010/09/21 00:19:08 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_CPP_PARSE_TEST_FAILURES = \
	cpp-line-02 \
	cpp-line-03

HACKT_CPP_PARSE_TEST_PASSES = \
	cpp-line-01 \
	cpp-line-04

# other depend source files for testing
EXTRA_DIST += \
	cpp-line-01-a.hac-cpp \
	cpp-line-02-a.hac-cpp \
	cpp-line-04-a.hac-cpp

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
	line-number-01 line-number-02 line-number-03 \
	bin-02 \
	quad-02 \
	bad-ref-01 \
	flat-import-03 \
	file-expand-01 file-expand-02 file-expand-03 file-expand-04 \
	file-expand-05

# these tests don't work when building in the srcdir
if SRCDIR_BUILD
import-006.hacktcmplfailtest:
	@$(SKIP_TEST_SCRIPT)
import-010.hacktcmplfailtest:
	@$(SKIP_TEST_SCRIPT)
endif

HACKT_PARSE_TEST_PASSES = \
	comment-003 comment-004 comment-005 comment-006 \
	comment-010 \
	import-004 import-012 import-013 import-015-t \
	hex-01 hex-04 \
	bin-01 \
	quad-01 \
	flat-import-01 flat-import-02 \
	pstring-01 pstring-02 \
	escaped-id-001 \
	prepend-01 prepend-01b prepend-02 prepend-03

HACKT_FLATTEN_TEST_PASSES = \
	badid-000 badid-001 \
	comment-003 comment-004 comment-005 comment-006 \
	comment-007 comment-008 \
	string-002 string-003 \
	import-000 import-004 import-012 import-013 \
	import-014-a import-014-b import-014-c \
	import-015-t import-022 \
	empty2 empty3 \
	flat-import-01 flat-import-02 \
	prepend-01 prepend-01b prepend-02 prepend-03

# pass flattening and parsing
HACKT_FLATTEN_PARSE_TEST_PASSES = \
	comment-003 comment-004 comment-005 comment-006 \
	import-004 \
	import-012 import-013 \
	import-015-t

# pass flattening, but not parsing
HACKT_FLATTEN_PARSE_TEST_FAILURES = \
	badid-000 badid-001 \
	string-002 string-003 \
	import-014-c

# import-000: lex fails, so error output differs

#	comment-007 comment-008
#	import-014-a import-014-b 
#	import-022
#	empty2 empty3


HACKT_FLATTEN_TEST_FAILURES = \
	comment-000 comment-001 comment-002 \
	comment-009 \
	string-001 \
	import-001 import-002 import-003 import-005 import-006 \
	import-007 import-008 import-009 import-010 import-011 \
	import-016 import-017 import-018 import-019 import-020 \
	import-021

# these tests don't work when building in the srcdir
if SRCDIR_BUILD
import-006.hacflatfailtest:
	@$(SKIP_TEST_SCRIPT)
import-010.hacflatfailtest:
	@$(SKIP_TEST_SCRIPT)
endif

# these don't compile without -i (include), so let compile test overwrite
prepend-02.depend:
	@$(TOUCH) $@
prepend-03.depend:
	@$(TOUCH) $@

# should be the subset of HACKT_PARSE_TEST_SUBJECTS that type-checks
# HACKT_OBJ_TEST_SUBJECTS = $(HACKT_PARSE_TEST_PASSES)

HACKT_CREATE_TEST_PASSES = \
	comment-010 \
	pstring-01 pstring-02 \
	escaped-id-001 \
	prepend-02 prepend-03

###############################################################################
# test for include path ordering

HACKT_PARSE_TEST_PASSES += include-path-01
HACKT_FLATTEN_TEST_PASSES += include-path-01
HACKT_FLATTEN_PARSE_TEST_PASSES += include-path-01
# and to confirm correct compilation
HACKT_CREATE_TEST_PASSES += include-path-01
# these files are *built*, but not distributed
INCLUDE_FIRST_TEST_FILES = me_first.hac include/me_first.hac
# extra deps
include-path-01.depend include-path-01.haco include-path-01.hacf: \
	$(INCLUDE_FIRST_TEST_FILES)

if SRCDIR_BUILD
include-path-01.hacfobjdifftest:
	@$(SKIP_TEST_SCRIPT)
include-path-01.hacktcreatetest:
	@$(SKIP_TEST_SCRIPT)
endif

include/me_first.hac: Makefile
	$(mkdir_p) include && \
	$(ECHO) "bool subdir;" > $@

me_first.hac: Makefile
	$(ECHO) "bool localdir;" > $@

CLEANFILES += $(INCLUDE_FIRST_TEST_FILES)
DISTHOOK_CLEANFILES += $(INCLUDE_FIRST_TEST_FILES)
# DISTCLEANDIRS += include

###############################################################################
# additional sources included by other test cases
srcdir_EXTRA_DIST += empty.hac bogus.hac \
	import-014-a.hac import-014-b.hac \
	import-015-b.hac import-015-c.hac import-015-e.hac \
	import-015-g.hac import-015-p.hac

# more output expectation files for when bison is used
BISON_OUTPUTS += string-002 string-003 \
	import-014-c cpp-line-02 \
	bad-ref-01 \
	flat-import-03 \
	file-expand-01 file-expand-02 file-expand-03 file-expand-04 \
	file-expand-05

include $(top_srcdir)/test/Make.test-tail

