# "dox/chpsim/Makefile.am"
#	$Id: Makefile.am,v 1.7 2007/08/15 02:48:32 fang Exp $

PDFS = hacchpsim.pdf
# deprecated:
noinst_DATA = chpsim-tex.pdf chpsim-tex.ps
PSS = $(PDFS:.pdf=.ps)

.NOTPARALLEL: chpsim-tex.pdf chpsim-tex.dvi

include $(top_srcdir)/dox/Make.dox-head

# have to add these by hand? :S
EXTRA_DIST += \
	chpsim-tex.tex \
	intro.tex \
	execution.tex

################### texinfo documentation #####################################

info_TEXINFOS += hacchpsim.texi
# install doesn't know how to handle generated html *directory* :(
# html_DATA = hacchpsim.html
HTML_HOOKS = hacchpsim.html
noinst_DATA += $(HTML_HOOKS)
# workaround: use install hooks
install-data-hook: $(HTML_HOOKS)
	$(mkdir_p) $(DESTDIR)$(htmldir) ; \
	for d in $^ ; do \
	  $(CP) -r $$d $(DESTDIR)$(htmldir)/$$d ; \
	done

uninstall-hook:
	for d in $(HTML_HOOKS) ; do \
	  $(RM) -r $(DESTDIR)$(htmldir)/$$d ; \
	done

hacchpsim_INCLUDES = \
	intro.texi \
	usage.texi \
	commands.texi \
	extending.texi \
	execution.texi \
	stdchpfn.texi

EXTRACT_TEXI_INCLUDES = $(GREP) "^@include" | $(CUT) -d\  -f2 | \
	  $(AWK) '{printf(" \\\n\t" $$1);}'

# HACCHPSIM_OPTIONS_TEXI = ...
# HACCHPSIM_COMMANDS_TEXI = ...
# HACCHPSIM_FUNCTIONS_TEXI = ...
# source-extracted texinfo files: (can we deduce this automatically? done.)
$(srcdir)/options.mk: usage.texi
	{ $(ECHO) -n "HACCHPSIM_OPTIONS_TEXI = " ; \
	$(CAT) $< | $(EXTRACT_TEXI_INCLUDES) ; \
	$(ECHO) "" ; \
	$(ECHO) "" ;} > $@

$(srcdir)/commands.mk: commands.texi
	{ $(ECHO) -n "HACCHPSIM_COMMANDS_TEXI = " ; \
	$(CAT) $< | $(EXTRACT_TEXI_INCLUDES) ; \
	$(ECHO) "" ; \
	$(ECHO) "" ;} > $@

$(srcdir)/functions.mk: stdchpfn.texi
	{ $(ECHO) -n "HACCHPSIM_FUNCTIONS_TEXI = " ; \
	$(CAT) $< | $(EXTRACT_TEXI_INCLUDES) ; \
	$(ECHO) "" ; \
	$(ECHO) "" ;} > $@

# These are typically only built by the maintainer, since they are distributed.
$(srcdir)/options.mk: Makefile
$(srcdir)/commands.mk: Makefile
$(srcdir)/functions.mk: Makefile

-include $(srcdir)/options.mk
-include $(srcdir)/commands.mk
-include $(srcdir)/functions.mk
AUTO_TEXINFO_DEPS = options.mk commands.mk functions.mk
EXTRA_DIST += $(AUTO_TEXINFO_DEPS)
AUTO_IGNORE += $(AUTO_TEXINFO_DEPS)
MAINTAINERCLEANFILES = $(AUTO_TEXINFO_DEPS)
# these could be CVS cleaned too...

$(HACCHPSIM_OPTIONS_TEXI): $(top_srcdir)/src/main/chpsim.cc
	$(EXTRACT_TEXINFO) $<

$(HACCHPSIM_COMMANDS_TEXI): $(top_srcdir)/src/sim/chpsim/Command.cc
	$(EXTRACT_TEXINFO) $<

CHPFN_SRCDIR = $(top_srcdir)/lib/libchpfn
# should be synchronized with list of all sources files in lib/libchpfn/
STDCHPFN_SRCS = \
	$(CHPFN_SRCDIR)/assert.cc \
	$(CHPFN_SRCDIR)/conditional.cc

# just re-generate all of them when any one changes...
$(HACCHPSIM_FUNCTIONS_TEXI): $(STDCHPFN_SRCS)
	$(EXTRACT_TEXINFO) $(STDCHPFN_SRCS)

$(HACCHPSIM_OPTIONS_TEXI): options
$(HACCHPSIM_COMMANDS_TEXI): cmd
$(HACCHPSIM_FUNCTIONS_TEXI): fn

options:
	$(mkdir_p) $@

cmd:
	$(mkdir_p) $@

fn:
	$(mkdir_p) $@

AUTO_IGNORE += \
	options \
	cmd \
	fn

CLEANFILES += \
	$(HACCHPSIM_OPTIONS_TEXI) \
	$(HACCHPSIM_COMMANDS_TEXI) \
	$(HACCHPSIM_FUNCTIONS_TEXI)
EXTRA_DIST += $(hacchpsim_INCLUDES)

# not bothering with more sophisticated dependency tracking
hacchpsim.dvi hacchpsim.pdf hacchpsim.html $(srcdir)/hacchpsim.info: \
	$(hacchpsim_INCLUDES) \
	$(HACCHPSIM_OPTIONS_TEXI) \
	$(HACCHPSIM_COMMANDS_TEXI) \
	$(HACCHPSIM_FUNCTIONS_TEXI)

# Q: do figures need to be installed?

# AM_MAKEINFOFLAGS =

################# documentation figures #######################################
SOURCE_FIGS =
SOURCE_DOTS = \
	figs/chpsim_event_cycle.dot \
	figs/chpsim_event_algo.dot
BUILT_FIGS = $(SOURCE_DOTS:.dot=.fig)
XFIG_FIGURES = $(SOURCE_FIGS) $(BUILT_FIGS)
XFIG_TEX = $(XFIG_FIGURES:.fig=.figtex)
XFIG_PSTEX = $(XFIG_FIGURES:.fig=.ps)
XFIG_PSTEX_T = $(XFIG_FIGURES:.fig=.pstex_t)
XFIG_PDFTEX = $(XFIG_FIGURES:.fig=.pdf)
XFIG_PDFTEX_T = $(XFIG_FIGURES:.fig=.pdftex_t)
XFIG_EPS = $(XFIG_FIGURES:.fig=.eps)
XFIG_PNG = $(XFIG_FIGURES:.fig=.png)

all-figs: $(BUILT_FIGS)

# build the directory automatically
$(BUILT_FIGS) $(XFIG_TEX) $(XFIG_PSTEX) $(XFIG_PSTEX_T) \
	$(XFIG_PDFTEX) $(XFIG_PDFTEX_T) $(XFIG_EPS) $(XFIG_PNG): figs/.dirstamp

figs/.dirstamp:
	$(mkdir_p) figs && touch $@

DISTCLEANFILES += figs/.dirstamp
EXTRA_DIST += $(SOURCE_FIGS) $(SOURCE_DOTS)

AUTO_IGNORE += \
	chpsim_event_cycle.fig \
	chpsim_event_algo.fig

CLEANFILES += $(BUILT_FIGS)

# need GNU make for the following:
# automatically generated dependencies in this (built) dir
-include chpsim-tex.pdfdepend
-include chpsim-tex.dvidepend
AUTO_DEPENDS += chpsim-tex.pdfdepend chpsim-tex.dvidepend

COMMON_DEPS = $(srcdir)/../common/hac.bib $(XFIG_TEX)

chpsim-tex.pdf: $(COMMON_DEPS) $(XFIG_PDFTEX) $(XFIG_PDFTEX_T)
chpsim-tex.dvi: $(COMMON_DEPS) $(XFIG_PSTEX) $(XFIG_PSTEX_T)
# all-figs named so automake doesn't think we're overriding the targets' rule
# LIMITATION: choosing not to try to include the .pdftex_t for now
built-figs hacchpsim.dvi: $(XFIG_EPS)
built-figs hacchpsim.pdf hacchpsim.html: $(XFIG_PDFTEX)
built-figs hacchpsim.html: $(XFIG_PNG)
built-figs $(srcdir)/hacchpsim.info: $(XFIG_PNG)
# still not sure why html looks for .eps figures...

.PHONY: built-figs


$(srcdir)/figs/.cvsignore: $(srcdir)/.cvsignore
	$(CP) $< $@

include $(top_srcdir)/dox/Make.dox-tail

