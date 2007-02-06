# "dox/chpsim/Makefile.am"
#	$Id: Makefile.am,v 1.2 2007/02/06 21:36:25 fang Exp $

PDFS = chpsim.pdf
PSS = $(PDFS:.pdf=.ps)

include $(top_srcdir)/dox/Make.dox-head

# have to add these by hand? :S
EXTRA_DIST += \
	chpsim.tex \
	intro.tex \
	execution.tex \
	figs/build_path.in

DIST_FIGS =
DIST_DOTS = figs/chpsim_event_cycle.dot
BUILT_FIGS = $(DIST_DOTS:.dot=.fig)

figs: $(BUILT_FIGS)

EXTRA_DIST += $(DIST_FIGS) $(DIST_DOTS)

AUTO_IGNORE += figs/build_path \
	chpsim_event_cycle.fig

FIG_EPSS = $(BUILT_FIGS:.fig=.eps)
FIG_PDFS = $(BUILT_FIGS:.fig=.pdf)

CLEANFILES += figs/build_path $(BUILT_FIGS) $(FIG_EPSS) $(FIG_PDFS)

# need GNU make for the following:
# automatically generated dependencies in this (built) dir
# -include cast2hac.depend
# AUTO_DEPENDS += cast2hac.depend
-include chpsim.pdfdepend
-include chpsim.dvidepend
AUTO_DEPENDS += chpsim.pdfdepend chpsim.dvidepend

chpsim.pdf chpsim.dvi: $(srcdir)/../common/hac.bib

chpsim.pdf: $(FIG_PDFS)
chpsim.dvi: $(FIG_EPSS)

$(srcdir)/figs/.cvsignore: $(srcdir)/.cvsignore
	$(CP) $< $@

include $(top_srcdir)/dox/Make.dox-tail

