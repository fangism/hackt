# "dox/cast2hac/Makefile.am"
#	$Id: Makefile.am,v 1.2 2006/05/26 07:46:01 fang Exp $

PDFS = cast2hac.pdf
PSS = $(PDFS:.pdf=.ps)

include $(top_srcdir)/dox/Make.dox-head

# have to add these by hand? :S
EXTRA_DIST += \
	ChangeLog \
	cast2hac.tex

# need GNU make for the following:
# automatically generated dependencies in this (built) dir
-include cast2hac.depend

include $(top_srcdir)/dox/Make.dox-tail

