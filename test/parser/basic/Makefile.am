# "test/parser/basic/Makefile.am"
#	$Id: Makefile.am,v 1.32 2008/08/26 06:31:00 fang Exp $

include $(top_srcdir)/test/Make.test-head

HACKT_PARSE_TEST_FAILURES = \
	004 005 008 009 \
	010 011 012 013 017 018 \
	020 021 023 024 025 026 027 028 \
	oper-03 oper-05 \
	act-002

HACKT_PARSE_TEST_PASSES = \
	000 001 002 003 006 007 \
	014 015 016 019 \
	022 \
	oper-02 oper-04 \
	inst-attrib-01 inst-attrib-02 inst-attrib-03 inst-attrib-04 \
	pipe-stdin-01 pipe-stdin-02 \
	act-001

HACKT_FLATTEN_TEST_PASSES = \
	015 016 017 018 019 \
	020 021 022 023 \
	pipe-stdin-01 pipe-stdin-02

# override some default rules
pipe-stdin-01.haco: pipe-stdin-01.hac
	@$(HACKT_COMPILE_TEST_STDIN_SCRIPT)
pipe-stdin-02.haco: pipe-stdin-02.hac
	@$(HACKT_COMPILE_TEST_STDIN_SCRIPT)

pipe-stdin-01.hacf: pipe-stdin-01.hac
	@$(HACKT_FLATTEN_TEST_STDIN_SCRIPT)
pipe-stdin-02.hacf: pipe-stdin-02.hac
	@$(HACKT_FLATTEN_TEST_STDIN_SCRIPT)

pipe-stdin-01.compileobjdiff:
	$(TOUCH) $@
pipe-stdin-02.compileobjdiff:
	$(TOUCH) $@

pipe-stdin-01.creatediff: pipe-stdin-01.createstderr
pipe-stdin-02.creatediff: pipe-stdin-02.createstderr

HACKT_FLATTEN_PARSE_TEST_PASSES = \
	015 016 019 \
	022

HACKT_FLATTEN_PARSE_TEST_FAILURES = \
	017 018 \
	020 023
# 021: parse failure vs. lex failures difference

HACKT_PARSE_TEST_WARNINGS = \
	oper-01
# NOTE: oper-01 passes with a warning for now

# should be the subset of HACKT_OBJ_TEST_SUBJECTS that passes unroll checks

HACKT_CREATE_TEST_PASSES = \
	$(HACKT_OBJ_TEST_SUBJECTS)

HACKT_CREATE_TEST_FAILURES =

# more output expectation files for when bison is used
BISON_OUTPUTS += 004 005 008 010 017 018 \
	020 021 026 027 028 \
	oper-05

srcdir_EXTRA_DIST += TODO

include $(top_srcdir)/test/Make.test-tail

