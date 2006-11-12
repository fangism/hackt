# "dox/objfmt/Makefile.am"
#	$Id: Makefile.am,v 1.8 2006/11/12 05:57:50 fang Exp $

PDFS = objfmt.pdf
PSS = $(PDFS:.pdf=.ps)

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
-include objfmt.depend
AUTO_DEPENDS += objfmt.depend

include $(top_srcdir)/dox/Make.dox-tail

