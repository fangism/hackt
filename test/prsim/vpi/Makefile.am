# "test/prsim/vpi/Makefile.am"
#	$Id: Makefile.am,v 1.16 2010/06/14 00:22:34 fang Exp $
# Test auto-Makefile for prsim with VPI.  

include $(top_srcdir)/test/Make.test-head
include $(top_srcdir)/installdirs.mk
# for examplesdir

# for .hac and .haco
HACKT_PRSIMRC_TEST_SUBJECTS = inverters \
	inverters-backwards \
	interleave-a interleave-b interleave-c \
	oscillator \
	channel-source-sink \
	channel-ledr-source-sink \
	celem \
	startup \
	and_tree and_template_tree \
	and_tree_reverse and_template_tree_reverse \
	inverters-separate \
	unstable-vacuous \
	flop_it

# for .v verilog tests
HACKT_VPI_TEST_SCRIPTS_PASSES = \
	inverters inverters-delay inverters-syntax-err \
	inverters-backwards \
	shoelace shoelace-debug \
	inverters-watch inverters-watch2 \
	initial-value \
	startup \
	interleave-a interleave-b interleave-c \
	oscillator-fanout \
	independent-01 \
	channel-source-sink \
	channel-source-sink-b \
	channel-source-sink-c \
	channel-expect-fail \
	channel-ledr-source-sink \
	channel-ledr-expect-fail \
	and_tree and_template_tree \
	and_tree_reverse and_template_tree_reverse \
	fatal-assert-fail \
	unstable-dequeue \
	unstable-vacuous \
	celem-test \
	flop_it flop_it-b flop_it-c

HACKT_VWRAP_TEST_SCRIPTS_PASSES = \
	standard standard-ports standard-reverse \
	param parse-test

EXAMPLE_NAME = vpiprsim-inverters
CLEANDIRS += $(EXAMPLE_NAME)

# install example from test
vpiinvdir = $(examplesdir)/$(EXAMPLE_NAME)

# installcheck
INSTALLCHECK_TARGETS = inverters.vxi-out inverters.vxi-subdir-out
CLEANFILES += inverters.vxi-subdir-out

if HAVE_VCS
installcheck-local: $(INSTALLCHECK_TARGETS)
	for o in '$(INSTALLCHECK_TARGETS)' ; \
	do $(CAT) $$o ; done
endif

Makefile.copy: example.mk Makefile
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
startup.vx: pli.tab
flop_it.vx: standard.v-wrap pli.tab
flop_it-b.vx: standard.v-wrap pli.tab
flop_it-c.vx: standard.v-wrap pli.tab

subdir/channel-values-X.txt:
	mkdir -p subdir && \
	{ $(ECHO) "# $@" ; \
	$(ECHO) "# bunch values" ; \
	$(ECHO) "0" ; \
	$(ECHO) "1" ; $(ECHO) "1" ; \
	$(ECHO) "0" ; $(ECHO) "0" ; $(ECHO) "0" ; \
	$(ECHO) "1" ; $(ECHO) "1" ; $(ECHO) "1" ; $(ECHO) "1" ; \
		} > $@
CLEANFILES += subdir/channel-values-X.txt

channel-source-sink-b.vx-out: subdir/channel-values-X.txt
channel-source-sink-c.vx-out: subdir/channel-values-X.txt

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
	inverters.hac inverters.v inverters-delay.v \
	shoelace.v shoelace-debug.v \
	channel-source-sink.hac channel-source-sink.v \
	channel-source-sink-b.v channel-source-sink-c.v \
	channel-ledr-source-sink.hac channel-ledr-source-sink.v \
	and_tree.hac and_tree.v standard.v startup.v startup.hac \
	and_tree_reverse.hac and_tree_reverse.v standard-reverse.v \
	and_template_tree.hac and_template_tree.v \
	and_template_tree_reverse.hac and_template_tree_reverse.v \
	oscillator.hac oscillator-fanout.v \
	flop_it.hac flop_it.v flop_it-b.v flop_it-c.v
EXTRA_DIST += $(EXAMPLE_DATA_FILES)
vpiinv_DATA = $(EXAMPLE_DATA_FILES) Makefile.copy

include $(top_srcdir)/test/Make.test-tail

