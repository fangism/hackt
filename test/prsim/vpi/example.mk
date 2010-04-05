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

.SUFFIXES: .v .vx .vx-log .v-wrap

include $(pkgdatadir)/mk/hackt.mk

EXTRACT_VCS_FLAGS_PIPE = grep "@vcs-flags@" | sed 's/^.*@vcs-flags@[ ]*//g'

.v.vx:
	+vflags=`cat $< | $(EXTRACT_VCS_FLAGS_PIPE)` && \
	$(VCS_ENV) $(VCS) $(VCS_FLAGS) $(VPI_FLAGS) $(PLI_FLAGS) $$vflags \
		-o $@ $< && \
	touch $@

.v.v-wrap:
	{ echo "\`include \"$<\"" ; echo "" ; \
	awk -f $(bindir)/wrap_verilog_modules_to_hacprsim.awk $< ;} > $@

.vx.vx-log:
	$(VPI_ENV) ./$< > $@ 2>&1

all: inverters.vx inverters-delay.vx oscillator-fanout.vx \
	shoelace.vx channel-source-sink.vx and_tree.vx \
	and_template_tree.vx and_tree_reverse.vx \
	and_template_tree_reverse.vx

# invert the direction, empty verilog, implementation in HAC
standard-reverse.v-wrap: standard-reverse.v
	{ echo "\`include \"$<\"" ; echo "" ; \
	awk -f $(bindir)/wrap_verilog_modules_to_hacprsim.awk \
		-v wrapper_ports=1 -v reverse=1 $< ;} > $@

# extra deps
and_tree.vx: standard.v-wrap pli.tab
and_template_tree.vx: standard.v-wrap pli.tab
and_tree_reverse.vx: standard-reverse.v-wrap pli.tab
and_template_tree_reverse.vx: standard-reverse.v-wrap pli.tab
inverters.vx-log: inverters.haco-c
inverters-delay.vx-log: inverters.haco-c
shoelace.vx-log: inverters.haco-c
channel-source-sink.vx-log: channel-source-sink.haco-c
and_tree.vx-log: and_tree.haco-c
and_template_tree.vx-log: and_template_tree.haco-c
and_tree_reverse.vx-log: and_tree_reverse.haco-c
and_template_tree_reverse.vx-log: and_template_tree_reverse.haco-c
oscillator-fanout.vx-log: oscillator.haco-c

pli.tab:
	echo "acc=wn:*" > $@

# .NOTPARALLEL: check
check: inverters.vx-log inverters-delay.vx-log oscillator-fanout.vx-log \
	shoelace.vx-log channel-source-sink.vx-log and_tree.vx-log \
	and_template_tree.vx-log and_tree_reverse.vx-log \
	and_template_tree_reverse.vx-log
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

