# "test/parser/datatype/Makefile.am"
#	$Id: Makefile.am,v 1.23 2011/03/23 00:38:32 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = \
	    003 004 005 009 \
	010 011 012 014 015 017 018 019 \
	020 024 025 \
	033 034 035 \
	040 041
# NOTE: 033 034 035 were moved to expect pass, b/c deftypes are now 'processes'
# TODO: remove {033,034,035}.stderr{,-bison} from CVS

HACKT_PARSE_TEST_PASSES = \
	001 002 006 007 008 \
	013 016 \
	021 022 023 026 027 028 029 \
	030 031 037 038 038b 039 \
	042 043 044 045 046 046 047 048 \
	050 051 052 053
# pending: act-00{1,2,3,4,5}

HACKT_CREATE_TEST_FAILURES = \
	037 \
	047 \
	053

# omitted tests are uninteresting
HACKT_CREATE_TEST_PASSES = \
	001 002 006 007 008 \
	023 026 029 \
	030 031 038 038b 039 \
	042 043 044 045 046 048 \
	050 051 052

# REGRESSION (parse, unroll, create): 032 036 
# (instantiations of 031 033, respectively)
# New on branch revision: HACKT-00-01-04-main-00-80-63-chpsim-01-23-expr-01
# We have disabled used of user-defined datatypes
# until they are well-defined in the language
# (They were a nuisance to maintain because datatypes
# don't have clear value or reference semantics.)

HACKT_ALLOC_TEST_PASSES = $(HACKT_CREATE_TEST_PASSES)

HACKT_CFLAT_TEST_SUBJECTS = \
	038 038b \
	042 043

# more output expectation files for when bison is used
BISON_OUTPUTS += 015 017 018 019 020 \
	033 034 035 040 041

EXTRA_DIST += \
	act-001.hac act-002.hac act-003.hac act-004.hac act-005.hac
# some of these tests pending update in ACT tests...

include $(top_srcdir)/test/Make.test-tail

