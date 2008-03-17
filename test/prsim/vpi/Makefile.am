# "test/prsim/vpi/Makefile.am"
#	$Id: Makefile.am,v 1.2 2008/03/17 23:10:49 fang Exp $
# Test auto-Makefile for prsim with VPI.  

include $(top_srcdir)/test/Make.test-head
include $(top_srcdir)/installdirs.mk
# for examplesdir

# for .hac and .haco
HACKT_PRSIMRC_TEST_SUBJECTS = inverters \
	interleave-a interleave-b interleave-c \
	oscillator \
	channel-source-sink

# for .v verilog tests
HACKT_VPI_TEST_SCRIPTS_PASSES = inverters shoelace \
	interleave-a interleave-b interleave-c \
	independent-01 \
	channel-source-sink

EXAMPLE_NAME = vpiprsim-inverters
CLEANDIRS += $(EXAMPLE_NAME)

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

Makefile.copy: example.mk
	$(SED) -e 's|@pkgdatadir@|$(pkgdatadir)|g' \
		-e 's|@vcs_flags@|$(INSTALLCHECK_VCS_FLAGS)|g' \
		-e 's|@vpi_flags@|$(INSTALLCHECK_VCS_VPI_FLAGS)|g' \
		-e 's|@vpi_env@|$(VPI_INSTALLCHECK_ENV)|g' \
		-e '/^VPI_ENV/s|\$$|$$$$|g' \
		$< > $@

# force: always re-run this test
inverters.vxi-subdir-out: Makefile.copy force
	+$(ECHO) "Part 2: building subdir to test installed example." && \
	$(RM) -r $(EXAMPLE_NAME) && \
	$(CP) -r $(vpiinvdir) . && \
	$(MV) $(EXAMPLE_NAME)/Makefile.copy $(EXAMPLE_NAME)/Makefile && \
	( cd $(EXAMPLE_NAME) && $(MAKE) check ) > $@

CLEANFILES += Makefile.copy

# TODO: provide Makefile?
dist_vpiinv_DATA = example.mk clkgen.v \
	inverters.hac inverters.v shoelace.v \
	channel-source-sink.hac channel-source-sink.v
vpiinv_DATA = Makefile.copy

include $(top_srcdir)/test/Make.test-tail

