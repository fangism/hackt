# "dox/util/Makefile.am"
#	$Id: Makefile.am,v 1.6 2006/11/12 05:57:56 fang Exp $

PDFS = fangutil.pdf
PSS = $(PDFS:.pdf=.ps)

include $(top_srcdir)/dox/Make.dox-head

EXTRA_DIST += fangutil.tex fangutil.bib \
	chapters/pointers.tex \
	chapters/persistent.tex

# need GNU make for the following:
# automatically generated dependencies in this (built) dir
-include fangutil.depend
AUTO_DEPENDS += fangutil.depend

include $(top_srcdir)/dox/Make.dox-tail

