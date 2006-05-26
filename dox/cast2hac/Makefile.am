# "dox/cast2hac/Makefile.am"
#	$Id: Makefile.am,v 1.1 2006/05/26 07:26:53 fang Exp $

PDFS = cast2hac.pdf
PSS = $(PDFS:.pdf=.ps)

include $(top_srcdir)/dox/Make.dox-head

# have to add these by hand? :S
EXTRA_DIST += \
	ChangeLog

# need GNU make for the following:
# automatically generated dependencies in this (built) dir
-include cast2hac.depend

include $(top_srcdir)/dox/Make.dox-tail

