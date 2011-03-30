# "test/net/verilog/Makefile.am"
#	$Id: Makefile.am,v 1.2 2011/03/30 04:19:10 fang Exp $
# Test auto-Makefile for hacknet -f output_format=verilog.  

include $(top_srcdir)/test/Make.test-head

# SUBDIRS =

# .hac inputs
HACVERILOG_TEST_SCRIPTS_PASSES = \
	001 002 003 004 005

HACVERILOG_TEST_SCRIPTS_FAILURES =

HACKT_PARSE_TEST_PASSES = \
	001 002 003 004 005

HACKT_PARSE_TEST_FAILURES =

# CLEANFILES +=
# AUTO_IGNORE +=
# EXTRA_DIST +=

HACKT_CREATE_TEST_PASSES = \
	001 002 003 004 005

HACKT_CREATE_TEST_FAILURES =

HACKT_ALLOC_TEST_PASSES =

# HACKT_CFLAT_TEST_SUBJECTS =

include $(top_srcdir)/test/Make.test-tail

# this directory: hacknet verilog mode only
AM_HACKNET_FLAGS += -c verilog.hacknet-conf
srcdir_EXTRA_DIST += verilog.hacknet-conf channel.hac

