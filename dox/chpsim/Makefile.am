# "dox/chpsim/Makefile.am"
#	$Id: Makefile.am,v 1.5 2007/07/31 23:22:30 fang Exp $

PDFS = hacchpsim.pdf
# deprecated:
noinst_DATA = chpsim-tex.pdf chpsim-tex.ps
PSS = $(PDFS:.pdf=.ps)

include $(top_srcdir)/dox/Make.dox-head

# have to add these by hand? :S
EXTRA_DIST += \
	chpsim-tex.tex \
	intro.tex \
	execution.tex \
	figs/build_path.in

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
	execution.texi

EXTRA_DIST += $(hacchpsim_INCLUDES)

# not bothering with more sophisticated dependency tracking
hacchpsim.dvi hacchpsim.pdf hacchpsim.html hacchpsim.info: $(hacchpsim_INCLUDES)

# Q: do figures need to be installed?

# AM_MAKEINFOFLAGS =

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


figs: $(BUILT_FIGS)

EXTRA_DIST += $(SOURCE_FIGS) $(SOURCE_DOTS)

AUTO_IGNORE += figs/build_path \
	chpsim_event_cycle.fig \
	chpsim_event_algo.fig

CLEANFILES += figs/build_path $(BUILT_FIGS)

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
# still not sure why html looks for .eps figures...

.PHONY: built-figs


$(srcdir)/figs/.cvsignore: $(srcdir)/.cvsignore
	$(CP) $< $@

include $(top_srcdir)/dox/Make.dox-tail

