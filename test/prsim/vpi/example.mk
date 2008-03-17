# "example.mk"
# To compile the "inverters" VPI example, you can use this template

# define these to use the Makefile
# VPATH = @srcdir@
# srcdir = @srcdir@
srcdir = .
pkgdatadir = @pkgdatadir@

# generic definitions
VCS = vcs
VCS_ENV =
VCS_FLAGS = @vcs_flags@
VPI_FLAGS = @vpi_flags@
VPI_ENV = @vpi_env@

.SUFFIXES: .v .vx .vx-log

include $(pkgdatadir)/mk/hackt.mk

.v.vx:
	+$(VCS_ENV) $(VCS) $(VCS_FLAGS) $(VPI_FLAGS) -o $@ $<

.vx.vx-log:
	$(VPI_ENV) ./$< > $@ 2>&1

all: inverters.vx shoelace.vx channel-source-sink.vx

# extra deps
inverters.vx-log: inverters.haco-a
shoelace.vx-log: inverters.haco-a
channel-source-sink.vx-log: channel-source-sink.haco-a
# really, is only run-time dep, not build-time dep

# .NOTPARALLEL: check
check: inverters.vx-log shoelace.vx-log channel-source-sink.vx-log
	cat $^
#	for f in $^ ; do cat $$f ; done

clean:
	rm -f *.haco*
	rm -f *.vx
	rm -rf *.vx.*
	rm -rf *csrc
	rm -f *.vx-log

