# "test/prsim/vpi/Makefile.am"
#	$Id: Makefile.am,v 1.6 2008/12/01 20:27:40 fang Exp $
# Test auto-Makefile for prsim with VPI.  

include $(top_srcdir)/test/Make.test-head
include $(top_srcdir)/installdirs.mk
# for examplesdir

# for .hac and .haco
HACKT_PRSIMRC_TEST_SUBJECTS = inverters \
	interleave-a interleave-b interleave-c \
	oscillator \
	channel-source-sink \
	and_tree

# for .v verilog tests
HACKT_VPI_TEST_SCRIPTS_PASSES = \
	inverters inverters-delay shoelace \
	interleave-a interleave-b interleave-c \
	independent-01 \
	channel-source-sink \
	and_tree

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

Makefile.copy: example.mk Makefile.am
	$(SED) -e 's|@pkgdatadir@|$(pkgdatadir)|g' \
		-e 's|@install_bindir@|$(bindir)|g' \
		-e 's|@vcs_flags@|$(INSTALLCHECK_VCS_FLAGS)|g' \
		-e 's|@vpi_flags@|$(INSTALLCHECK_VCS_VPI_FLAGS)|g' \
		-e 's|@vpi_env@|$(VPI_INSTALLCHECK_ENV)|g' \
		-e '/^VPI_ENV/s|\$$|$$$$|g' \
		$< > $@

# PLI tab file
pli.tab:
	$(ECHO) "acc=wn:*" > $@

PLI_FLAGS = -P pli.tab

# custom rule
and_tree.vx: and_tree.v standard.v-wrap pli.tab
if HAVE_VPI
	+$(VCS_ENV) $(VCS) $(AM_VCS_FLAGS) $(AM_VCS_VPI_FLAGS) $(PLI_FLAGS) -o $@ $<
else
	@$(ECHO) "VPI not explicitly enabled during configure."
endif


# force: always re-run this test
inverters.vxi-subdir-out: Makefile.copy force
	+$(ECHO) "Part 2: building subdir to test installed example." && \
	$(RM) -r $(EXAMPLE_NAME) && \
	$(CP) -r $(vpiinvdir) . && \
	$(MV) $(EXAMPLE_NAME)/Makefile.copy $(EXAMPLE_NAME)/Makefile && \
	( cd $(EXAMPLE_NAME) && \
		PATH=$(bindir)$(PATH_SEPARATOR)$$PATH $(MAKE) check ) > $@

CLEANFILES += Makefile.copy pli.tab

EXAMPLE_DATA_FILES = example.mk clkgen.v \
	inverters.hac inverters.v inverters-delay.v shoelace.v \
	channel-source-sink.hac channel-source-sink.v \
	and_tree.hac and_tree.v standard.v
EXTRA_DIST += $(EXAMPLE_DATA_FILES)
vpiinv_DATA = $(EXAMPLE_DATA_FILES) Makefile.copy

include $(top_srcdir)/test/Make.test-tail

