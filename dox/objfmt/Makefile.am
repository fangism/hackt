# "dox/objfmt/Makefile.am"
#	$Id: Makefile.am,v 1.4.20.2 2005/07/23 04:27:55 fang Exp $

include $(top_srcdir)/dox/Make.dox-head

PDFS = objfmt.pdf
PSS = $(PDFS:.pdf=.ps)

# all-local: pdf-local ps-local

pdf-local: $(PDFS)

ps-local: $(PSS)

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

include $(top_srcdir)/dox/Make.dox-tail

