# "dox/chpsim/Makefile.am"
#	$Id: Makefile.am,v 1.4.6.2 2007/07/30 08:44:53 fang Exp $

PDFS = chpsim.pdf
# old:
noinst_DATA = chpsim-tex.pdf chpsim-tex.ps
PSS = $(PDFS:.pdf=.ps)

include $(top_srcdir)/dox/Make.dox-head

# have to add these by hand? :S
EXTRA_DIST += \
	chpsim-tex.tex \
	intro.tex \
	execution.tex \
	figs/build_path.in

info_TEXINFOS += chpsim.texi
# install doesn't know how to handle generated html *directory* :(
# html_DATA = chpsim.html
noinst_DATA += chpsim.html
# workaround: use install hooks
install-data-hook: chpsim.html
	$(mkdir_p) $(DESTDIR)$(htmldir) ; \
	for d in $^ ; do \
	  $(CP) -r $$d $(DESTDIR)$(htmldir)/$$d ; \
	done

uninstall-hook:
	$(RM) -r $(DESTDIR)$(htmldir)/chpsim.html

chpsim_INCLUDES = \
	intro.texi \
	execution.texi \
	test.texi

EXTRA_DIST += $(chpsim_INCLUDES)

chpsim.dvi chpsim.pdf chpsim.html chpsim.info: $(chpsim_INCLUDES)

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
# -include cast2hac.depend
# AUTO_DEPENDS += cast2hac.depend
-include chpsim-tex.pdfdepend
-include chpsim-tex.dvidepend
AUTO_DEPENDS += chpsim-tex.pdfdepend chpsim-tex.dvidepend

COMMON_DEPS = $(srcdir)/../common/hac.bib $(XFIG_TEX)

chpsim-tex.pdf: $(COMMON_DEPS) $(XFIG_PDFTEX) $(XFIG_PDFTEX_T)
chpsim-tex.dvi: $(COMMON_DEPS) $(XFIG_PSTEX) $(XFIG_PSTEX_T)
# all-figs named so automake doesn't think we're overriding the targets' rule
# LIMITATION: choosing not to try to include the .pdftex_t for now
built-figs chpsim.dvi: $(XFIG_EPS)
built-figs chpsim.pdf chpsim.html: $(XFIG_PDFTEX)
built-figs chpsim.html: $(XFIG_PNG)
# still not sure why html looks for .eps figures...

.PHONY: built-figs


$(srcdir)/figs/.cvsignore: $(srcdir)/.cvsignore
	$(CP) $< $@

include $(top_srcdir)/dox/Make.dox-tail

