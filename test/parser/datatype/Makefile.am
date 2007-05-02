# "test/parser/datatype/Makefile.am"
#	$Id: Makefile.am,v 1.16.12.1 2007/05/02 23:42:48 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = \
	    003 004 005 009 \
	010 011 012 014 015 017 018 019 \
	020 024 025
# NOTE: 034 035 were moved to expect pass, since deftypes are now 'processes'

HACKT_PARSE_TEST_PASSES = \
	001 002 006 007 008 \
	013 016 \
	021 022 023 026 027 028 029 \
	030 031 033 034 035 037 038

if USE_UNROLL_PHASE
HACKT_UNROLL_TEST_PASSES = \
	001 002 006 007 008 \
	013 016 \
	021 022 023 026 027 028 029 \
	030 031 033

HACKT_UNROLL_TEST_FAILURES = \
	037
endif

# omitted tests are uninteresting
HACKT_CREATE_TEST_PASSES = \
	001 002 006 007 008 \
	023 026 029 \
	030 031 033 038

# REGRESSION (parse, unroll, create): 032 036 
# (instantiations of 031 033, respectively)
# New on branch revision: HACKT-00-01-04-main-00-80-63-chpsim-01-23-expr-01
# We have disabled used of user-defined datatypes
# until they are well-defined in the language
# (They were a nuisance to maintain because datatypes
# don't have clear value or reference semantics.)

if USE_UNROLL_PHASE
HACKT_CREATE_TEST_FAILURES =
else
HACKT_CREATE_TEST_FAILURES = $(HACKT_UNROLL_TEST_FAILURES)
endif

HACKT_ALLOC_TEST_PASSES = $(HACKT_CREATE_TEST_PASSES)

HACKT_CFLAT_TEST_SUBJECTS = \
	038

# more output expectation files for when bison is used
BISON_OUTPUTS += 015 017 018 019 020

include $(top_srcdir)/test/Make.test-tail

