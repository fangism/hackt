# "example.mk"
# To compile the "inverters" VPI example, you can use this template

# define these to use the Makefile
# VPATH = @srcdir@
# srcdir = @srcdir@
srcdir = .
pkgdatadir = @pkgdatadir@

# generic definitions
VCS = vcs
VCS_FLAGS = @vcs_flags@
VPI_FLAGS = @vpi_flags@
VPI_ENV = @vpi_env@

.SUFFIXES: .v .vx

include $(pkgdatadir)/mk/hackt.mk

.v.vx:
	+$(VCS) $(VCS_FLAGS) $(VPI_FLAGS) -o $@ $<

all: inverters.vx

# extra deps
inverters.vx: inverters.haco-a
# really, is only run-time dep, not build-time dep

check: inverters.vx
	$(VPI_ENV) ./$<

clean:
	rm -f inverters.vx

