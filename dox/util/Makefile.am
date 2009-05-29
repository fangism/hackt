# "dox/util/Makefile.am"
#	$Id: Makefile.am,v 1.9 2009/05/29 15:13:22 fang Exp $

PDFS = fangutil.pdf
DVIS = $(PDFS:.pdf=.dvi)
PSS = $(PDFS:.pdf=.ps)

.NOTPARALLEL: fangutil.pdf fangutil.dvi

include $(top_srcdir)/dox/Make.dox-head

EXTRA_DIST += fangutil.tex fangutil.bib \
	chapters/pointers.tex \
	chapters/persistent.tex

# need GNU make for the following:
# automatically generated dependencies in this (built) dir
# -include fangutil.depend
# AUTO_DEPENDS += fangutil.depend
-include fangutil.pdfdepend
-include fangutil.dvidepend
AUTO_DEPENDS += fangutil.pdfdepend fangutil.dvidepend

fangutil.dvi fangutil.pdf: $(srcdir)/fangutil.bib

include $(top_srcdir)/dox/Make.dox-tail

