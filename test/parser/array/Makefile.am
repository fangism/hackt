# "test/parser/array/Makefile.am"
#	$Id: Makefile.am,v 1.7.4.1 2005/08/07 01:07:30 fang Exp $

include $(top_srcdir)/test/Make.test-head

ARTC_TEST_FAILURES = \
	    003 004 005 006 008 009 \
	011 012 014 017 018 019 \
	020 021 022 023 024 025 026 027 028 \
	030 031 032 033 034 035 036

ARTC_TEST_PASSES = \
	001 002 007 \
	010 013 015 016 \
	029 \
	037 038 039 \
	040 041

# unfortunately is out of order... to bad $(sort ...) is non-posix
ARTC_TEST_SUBJECTS = $(ARTC_TEST_PASSES) $(ARTC_TEST_FAILURES)

ARTOBJ_TEST_SUBJECTS = \
	$(ARTC_TEST_PASSES)

ARTOBJUNROLL_TEST_SUBJECTS = \
	$(ARTOBJ_TEST_SUBJECTS)

include $(top_srcdir)/test/Make.test-tail

