# "dox/chpsim/Makefile.am"
#	$Id: Makefile.am,v 1.3.2.1 2007/06/06 05:02:10 fang Exp $

PDFS = chpsim.pdf
PSS = $(PDFS:.pdf=.ps)

include $(top_srcdir)/dox/Make.dox-head

# have to add these by hand? :S
EXTRA_DIST += \
	chpsim.tex \
	intro.tex \
	execution.tex \
	figs/build_path.in

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


figs: $(BUILT_FIGS)

EXTRA_DIST += $(SOURCE_FIGS) $(SOURCE_DOTS)

AUTO_IGNORE += figs/build_path \
	chpsim_event_cycle.fig \
	chpsim_event_algo.fig

CLEANFILES += figs/build_path $(BUILT_FIGS)

clean-local: clean-figs

clean-figs: force
	$(RM) figs/*.fig.orig
	$(RM) figs/*.figtex
	$(RM) figs/*.eps
	$(RM) figs/*.ps
	$(RM) figs/*.pdf
	$(RM) figs/*.pstex
	$(RM) figs/*.pdftex
	$(RM) figs/*.pstex_t
	$(RM) figs/*.pdftex_t

# need GNU make for the following:
# automatically generated dependencies in this (built) dir
# -include cast2hac.depend
# AUTO_DEPENDS += cast2hac.depend
-include chpsim.pdfdepend
-include chpsim.dvidepend
AUTO_DEPENDS += chpsim.pdfdepend chpsim.dvidepend

COMMON_DEPS = $(srcdir)/../common/hac.bib $(XFIG_TEX)

chpsim.pdf: $(COMMON_DEPS) $(XFIG_PDFTEX) $(XFIG_PDFTEX_T)
chpsim.dvi: $(COMMON_DEPS) $(XFIG_PSTEX) $(XFIG_PSTEX_T)

$(srcdir)/figs/.cvsignore: $(srcdir)/.cvsignore
	$(CP) $< $@

include $(top_srcdir)/dox/Make.dox-tail

