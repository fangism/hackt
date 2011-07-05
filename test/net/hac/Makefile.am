# "test/net/hac/Makefile.am"
#	$Id: Makefile.am,v 1.33 2011/04/29 18:37:11 fang Exp $
# Test auto-Makefile for spice-to-hac.  

include $(top_srcdir)/test/Make.test-head

# SUBDIRS = .

# .hac inputs
HACKNET_TEST_SCRIPTS_PASSES =
HACKNET_TEST_SCRIPTS_FAILURES =

EXTRA_DIST_HAC = \
	globals \
	config-arg-05 \
	top-type-ports-01

EXTRA_DIST += $(EXTRA_DIST_HAC:=.hac)

srcdir_EXTRA_DIST += \
	level_shift.spice-expect

HACKT_PARSE_TEST_PASSES = \
	level_shift

level_shift.haco: level_shift.spice-hac

HACKT_PARSE_TEST_FAILURES =

CLEANFILES +=
AUTO_IGNORE +=
EXTRA_DIST +=

HACKT_CREATE_TEST_PASSES = \
	level_shift

HACKT_CREATE_TEST_FAILURES =

HACKT_ALLOC_TEST_PASSES = \
	level_shift

HACKT_CFLAT_TEST_SUBJECTS = \
	level_shift

HACKT_CFLAT_SPRS_TEST_SUBJECTS =

include $(top_srcdir)/test/Make.test-tail

