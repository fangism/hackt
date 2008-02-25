# "test/prsim/vpi/Makefile.am"
#	$Id: Makefile.am,v 1.1.2.2 2008/02/25 19:55:23 fang Exp $
# Test auto-Makefile for prsim with VPI.  

include $(top_srcdir)/test/Make.test-head
include $(top_srcdir)/installdirs.mk
# for examplesdir

# for .hac and .haco
HACKT_PRSIM_TEST_SUBJECTS = inverters

# for .v verilog tests
HACKT_VPI_TEST_SCRIPTS_PASSES = inverters

EXAMPLE_NAME = vpiprsim-inverters

# install example from test
vpiinvdir = $(examplesdir)/$(EXAMPLE_NAME)

# installcheck
INSTALLCHECK_TARGETS = inverters.vxi-out inverters.vxi-subdir-out
CLEANFILES += inverters.vxi-subdir-out

if HAVE_VPI
installcheck-local: $(INSTALLCHECK_TARGETS)
	for o in '$(INSTALLCHECK_TARGETS)' ; \
	do $(CAT) $$o ; done
endif

inverters.vxi-subdir-out:
	+$(ECHO) "Part 2: building subdir to test installed example." && \
	$(RM) -r $(EXAMPLE_NAME) && \
	$(CP) -r $(vpiinvdir) . && \
	$(SED) -e 's|@pkgdatadir@|$(pkgdatadir)|g' \
		-e 's|@vcs_flags@|$(COMMON_VCS_FLAGS)|g' \
		-e 's|@vpi_flags@|$(INSTALLCHECK_VCS_VPI_FLAGS)|g' \
		-e 's|@vpi_env@|$(VPI_INSTALLCHECK_ENV)|g' \
		-e '/^VPI_ENV/s|\$$|$$$$|g' \
		$(srcdir)/example.mk > $(EXAMPLE_NAME)/Makefile && \
	( cd $(EXAMPLE_NAME) && $(MAKE) check ) > $@


# TODO: provide Makefile?
dist_vpiinv_DATA = clkgen.v inverters.hac inverters.v example.mk

# generalize and automate this rule
DISTCLEANDIRS += inverters.vx.daidir inverters.vxi.daidir

include $(top_srcdir)/test/Make.test-tail

