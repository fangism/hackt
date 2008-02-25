# "test/prsim/vpi/Makefile.am"
#	$Id: Makefile.am,v 1.1.2.1 2008/02/25 08:23:58 fang Exp $
# Test auto-Makefile for prsim with VPI.  

include $(top_srcdir)/test/Make.test-head

# for testing use
VCS_VPI_FLAGS = +vpi -use_vpiobj $(top_builddir)/src/.libs/vpihacprsim.so \
	-LDFLAGS="-L$(top_builddir)/src/.libs" 
VCS_FLAGS = +incdir+$(srcdir) +cli+3 \
	-Mupdate +v2k +vcsace+diff+ok $(VCS_VPI_FLAGS)
# -v $(srcdir)/clkgen.v

# for .hac and .haco
HACKT_PRSIM_TEST_SUBJECTS = inverters

# for .v verilog tests
HACKT_VPI_TEST_SCRIPTS_PASSES = inverters

# TODO: install check

srcdir_EXTRA_DIST += clkgen.v

# generalize and automate this rule
DISTCLEANDIRS += inverters.vx.daidir

include $(top_srcdir)/test/Make.test-tail

