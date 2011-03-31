# "test/net/verilog/Makefile.am"
#	$Id: Makefile.am,v 1.3 2011/03/31 01:21:54 fang Exp $
# Test auto-Makefile for hacknet -f output_format=verilog.  

include $(top_srcdir)/test/Make.test-head

# SUBDIRS =

# .hac inputs
HACVERILOG_TEST_SCRIPTS_PASSES = \
	001 002 003 004 005 006 007 008 009

HACVERILOG_TEST_SCRIPTS_FAILURES =

HACKT_PARSE_TEST_PASSES = \
	001 002 003 004 005 006 007 008 009

HACKT_PARSE_TEST_FAILURES =

# CLEANFILES +=
# AUTO_IGNORE +=
# EXTRA_DIST +=

HACKT_CREATE_TEST_PASSES = \
	001 002 003 004 005 006 007 008 009

HACKT_CREATE_TEST_FAILURES =

HACKT_ALLOC_TEST_PASSES =

# HACKT_CFLAT_TEST_SUBJECTS =

include $(top_srcdir)/test/Make.test-tail

# this directory: hacknet verilog mode only
AM_HACKNET_FLAGS += -c verilog.hacknet-conf
srcdir_EXTRA_DIST += verilog.hacknet-conf
srcdir_EXTRA_DIST += channel.hac globals.hac fifo.hac

