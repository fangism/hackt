# "dox/cast2hac/Makefile.am"
#	$Id: Makefile.am,v 1.8 2006/11/12 05:57:44 fang Exp $

PDFS = cast2hac.pdf
PSS = $(PDFS:.pdf=.ps)

include $(top_srcdir)/dox/Make.dox-head

# have to add these by hand? :S
EXTRA_DIST += \
	ChangeLog \
	cast2hac.tex \
	intro.tex \
	CHP.tex \
	PRS.tex \
	arrays.tex \
	connections.tex \
	definitions.tex \
	instances.tex \
	loops.tex \
	spec.tex \
	templates.tex \
	types.tex \
	typedefs.tex \
	cflat.tex

# need GNU make for the following:
# automatically generated dependencies in this (built) dir
-include cast2hac.depend
AUTO_DEPENDS += cast2hac.depend

include $(top_srcdir)/dox/Make.dox-tail

