# "dox/cast2hac/Makefile.am"
#	$Id: Makefile.am,v 1.10 2007/08/09 03:47:06 fang Exp $

PDFS = cast2hac.pdf
PSS = $(PDFS:.pdf=.ps)

.NOTPARALLEL: cast2hac.pdf cast2hac.dvi

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
# -include cast2hac.depend
# AUTO_DEPENDS += cast2hac.depend
-include cast2hac.pdfdepend
-include cast2hac.dvidepend
AUTO_DEPENDS += cast2hac.pdfdepend cast2hac.dvidepend

include $(top_srcdir)/dox/Make.dox-tail

