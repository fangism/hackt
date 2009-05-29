# "dox/objfmt/Makefile.am"
#	$Id: Makefile.am,v 1.11 2009/05/29 15:13:19 fang Exp $

PDFS = objfmt.pdf
DVIS = $(PDFS:.pdf=.dvi)
PSS = $(PDFS:.pdf=.ps)

.NOTPARALLEL: objfmt.pdf objfmt.dvi

include $(top_srcdir)/dox/Make.dox-head

# have to add these by hand? :S
EXTRA_DIST += objfmt.tex \
	chapters/intro.tex \
	chapters/skeleton.tex \
	chapters/root.tex \
	chapters/namespace.tex \
	chapters/definition.tex \
	chapters/instance.tex \
	chapters/value.tex \
	chapters/sequential.tex \
	chapters/common.tex \
	ChangeLog

# need GNU make for the following:
# automatically generated dependencies in this (built) dir
# -include objfmt.depend
# AUTO_DEPENDS += objfmt.depend
-include objfmt.pdfdepend
-include objfmt.dvidepend
AUTO_DEPENDS += objfmt.pdfdepend objfmt.dvidepend

include $(top_srcdir)/dox/Make.dox-tail

