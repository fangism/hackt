# "test/prsim/vpi/Makefile.am"
#	$Id: Makefile.am,v 1.13 2009/10/31 02:22:17 fang Exp $
# Test auto-Makefile for prsim with VPI.  

include $(top_srcdir)/test/Make.test-head
include $(top_srcdir)/installdirs.mk
# for examplesdir

# for .hac and .haco
HACKT_PRSIMRC_TEST_SUBJECTS = inverters \
	interleave-a interleave-b interleave-c \
	oscillator \
	channel-source-sink \
	and_tree and_template_tree \
	and_tree_reverse and_template_tree_reverse

# for .v verilog tests
HACKT_VPI_TEST_SCRIPTS_PASSES = \
	inverters inverters-delay shoelace \
	inverters-watch inverters-watch2 \
	interleave-a interleave-b interleave-c \
	oscillator-fanout \
	independent-01 \
	channel-source-sink \
	and_tree and_template_tree \
	and_tree_reverse and_template_tree_reverse \
	fatal-assert-fail

HACKT_VWRAP_TEST_SCRIPTS_PASSES = \
	standard standard-ports standard-reverse

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

# PLI_FLAGS = -P pli.tab

# additional dependencies
standard-ports.v: standard.v
	{ $(ECHO) "// @verilog-wrap-flags@ -v wrapper_ports=1" ; \
		$(CAT) $< ;} > $@

standard-reverse.v: standard.v
	{ $(ECHO) "// @verilog-wrap-flags@ -v reverse=1 -v wrapper_ports=1" ; \
		$(CAT) $< ;} > $@

and_tree.vx: standard.v-wrap pli.tab
and_template_tree.vx: standard.v-wrap pli.tab
and_tree_reverse.vx: standard-reverse.v-wrap pli.tab
and_template_tree_reverse.vx: standard-reverse.v-wrap pli.tab

# force: always re-run this test
inverters.vxi-subdir-out: Makefile.copy force
	+$(ECHO) "Part 2: building subdir to test installed example." && \
	$(RM) -r $(EXAMPLE_NAME) && \
	$(CP) -r $(vpiinvdir) . && \
	$(MV) $(EXAMPLE_NAME)/Makefile.copy $(EXAMPLE_NAME)/Makefile && \
	( cd $(EXAMPLE_NAME) && \
		PATH=$(bindir)$(PATH_SEPARATOR)$$PATH $(MAKE) check ) > $@

CLEANFILES += Makefile.copy pli.tab standard-ports.v standard-reverse.v

EXAMPLE_DATA_FILES = example.mk clkgen.v \
	inverters.hac inverters.v inverters-delay.v shoelace.v \
	channel-source-sink.hac channel-source-sink.v \
	and_tree.hac and_tree.v standard.v \
	and_tree_reverse.hac and_tree_reverse.v standard-reverse.v \
	and_template_tree.hac and_template_tree.v \
	and_template_tree_reverse.hac and_template_tree_reverse.v \
	oscillator.hac oscillator-fanout.v
EXTRA_DIST += $(EXAMPLE_DATA_FILES)
vpiinv_DATA = $(EXAMPLE_DATA_FILES) Makefile.copy

include $(top_srcdir)/test/Make.test-tail

