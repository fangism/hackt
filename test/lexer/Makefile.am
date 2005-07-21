# "test/lexer/Makefile.am"
#	$Id: Makefile.am,v 1.1.38.1 2005/07/21 01:06:26 fang Exp $

include $(top_srcdir)/test/Make.test-head

ARTC_TEST_FAILURES = \
	badid-000 badid-001

# ARTC_TEST_PASSES =

ARTC_TEST_SUBJECTS = $(ARTC_TEST_FAILURES)
# $(ARTC_TEST_PASSES)

# should be the subset of ART_TEST_SUBJECTS that type-checks
# ARTOBJ_TEST_SUBJECTS = $(ARTC_TEST_PASSES)

# should be the subset of ARTOBJ_TEST_SUBJECTS that passes unroll checks
# ARTOBJUNROLL_TEST_SUBJECTS = $(ARTOBJ_TEST_SUBJECTS)

TESTS = $(ARTC_TEST_SUBJECTS:=.artctest) \
	$(ARTOBJ_TEST_SUBJECTS:=.artobjtest) \
	$(ARTOBJUNROLL_TEST_SUBJECTS:=.artobjunrolltest)

# more output expectation files for when bison is used
# BISON_OUTPUTS +=

include $(top_srcdir)/test/Make.test-tail

