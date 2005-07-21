# "test/parser/template/Makefile.am"
#	$Id: Makefile.am,v 1.8.2.1 2005/07/21 01:06:28 fang Exp $

include $(top_srcdir)/test/Make.test-head

ARTC_TEST_FAILURES = \
	    003 005 \
	010 013 014 019 \
	020 023 024 025 027 028 \
	042 047 048 \
	050 051 052 053 054 \
	060 065 066 067 069

ARTC_TEST_PASSES = \
	001 002 004 006 007 008 009 \
	011 012 015 016 017 018 \
	021 022 026 029 \
	030 031 032 033 034 035 036 037 038 039 \
	040 041 043 044 045 046 049 \
	055 056 057 058 059 \
	061 062 063 064 068 \
	070 071 072 073

# KNOWN BUGS:
# should fail at unroll-time:
#	057 058 (not propagating relaxed formal dependence yet)

BISON_OUTPUTS += 027 028 042 047 048 050

ARTC_TEST_SUBJECTS = $(ARTC_TEST_FAILURES) $(ARTC_TEST_PASSES)

ARTOBJ_TEST_SUBJECTS = \
	$(ARTC_TEST_PASSES)

ARTOBJUNROLL_TEST_SUBJECTS = \
	$(ARTOBJ_TEST_SUBJECTS)

TESTS = $(ARTC_TEST_SUBJECTS:=.artctest) \
	$(ARTOBJ_TEST_SUBJECTS:=.artobjtest) \
	$(ARTOBJUNROLL_TEST_SUBJECTS:=.artobjunrolltest)

include $(top_srcdir)/test/Make.test-tail

