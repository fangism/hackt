# "dox/prsim/Makefile.am"
#	$Id: Makefile.am,v 1.2 2007/08/09 03:47:09 fang Exp $

PDFS = prsim.pdf
PSS = $(PDFS:.pdf=.ps)

.NOTPARALLEL: prsim.pdf prsim.dvi

include $(top_srcdir)/dox/Make.dox-head

# have to add these by hand? :S
EXTRA_DIST += \
	prsim.tex

# need GNU make for the following:
# automatically generated dependencies in this (built) dir
# -include cast2hac.depend
# AUTO_DEPENDS += cast2hac.depend
-include prsim.pdfdepend
-include prsim.dvidepend
AUTO_DEPENDS += prsim.pdfdepend prsim.dvidepend

include $(top_srcdir)/dox/Make.dox-tail

