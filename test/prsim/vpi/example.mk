# "example.mk"
# To compile the "inverters" VPI example, you can use this template

# define these to use the Makefile
# VPATH = @srcdir@
# srcdir = @srcdir@
srcdir = .
bindir = @install_bindir@
pkgdatadir = @pkgdatadir@

# generic definitions
VCS = vcs
VCS_ENV =
VCS_FLAGS = @vcs_flags@
VPI_FLAGS = @vpi_flags@
VPI_ENV = @vpi_env@
PLI_FLAGS =
MORE_PLI_FLAGS = $(PLI_FLAGS) -P pli.tab

.SUFFIXES: .v .vx .vx-log .v-wrap

include $(pkgdatadir)/mk/hackt.mk

.v.vx:
	+$(VCS_ENV) $(VCS) $(VCS_FLAGS) $(VPI_FLAGS) $(PLI_FLAGS) -o $@ $< && touch $@

.v.v-wrap:
	{ echo "\`include \"$<\"" ; echo "" ; \
	awk -f $(bindir)/wrap_verilog_modules_to_hacprsim.awk $< ;} > $@

.vx.vx-log:
	$(VPI_ENV) ./$< > $@ 2>&1

all: inverters.vx inverters-delay.vx oscillator-fanout.vx \
	shoelace.vx channel-source-sink.vx and_tree.vx

# special cases
and_tree.vx: and_tree.v standard.v-wrap pli.tab
	+$(VCS_ENV) $(VCS) $(VCS_FLAGS) $(VPI_FLAGS) $(MORE_PLI_FLAGS) -o $@ $< && touch $@


# extra deps
inverters.vx-log: inverters.haco-a
inverters-delay.vx-log: inverters.haco-a
shoelace.vx-log: inverters.haco-a
channel-source-sink.vx-log: channel-source-sink.haco-a
and_tree.vx-log: and_tree.haco-a
oscillator-fanout.vx-log: oscillator.haco-a

pli.tab:
	echo "acc=wn:*" > $@

# .NOTPARALLEL: check
check: inverters.vx-log inverters-delay.vx-log oscillator-fanout.vx-log \
	shoelace.vx-log channel-source-sink.vx-log and_tree.vx-log
	cat $^
#	for f in $^ ; do cat $$f ; done

clean:
	rm -f *.haco*
	rm -f *.v-wrap
	rm -f *.vx
	rm -rf *.vx.*
	rm -rf *csrc
	rm -f *.vx-log
	rm -f pli.tab

