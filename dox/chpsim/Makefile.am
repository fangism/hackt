# "dox/chpsim/Makefile.am"
#	$Id: Makefile.am,v 1.1 2007/01/24 04:14:46 fang Exp $

PDFS = chpsim.pdf
PSS = $(PDFS:.pdf=.ps)

include $(top_srcdir)/dox/Make.dox-head

# have to add these by hand? :S
EXTRA_DIST += \
	chpsim.tex

# need GNU make for the following:
# automatically generated dependencies in this (built) dir
# -include cast2hac.depend
# AUTO_DEPENDS += cast2hac.depend
-include chpsim.pdfdepend
-include chpsim.dvidepend
AUTO_DEPENDS += chpsim.pdfdepend chpsim.dvidepend

chpsim.pdf chpsim.dvi: $(srcdir)/../common/hac.bib

include $(top_srcdir)/dox/Make.dox-tail

