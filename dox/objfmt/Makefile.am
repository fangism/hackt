# "dox/objfmt/Makefile.am"
#	$Id: Makefile.am,v 1.3 2005/05/05 23:23:16 fang Exp $

include $(top_srcdir)/dox/mk/Make.latex

PDFS = objfmt.pdf
PSS = $(PDFS:.pdf=.ps)

# all-local: pdf-local ps-local

pdf-local: $(PDFS)

ps-local: $(PSS)

# have to add these by hand? :S
EXTRA_DIST = objfmt.tex \
	chapters/intro.tex \
	chapters/skeleton.tex \
	chapters/root.tex \
	chapters/namespace.tex \
	chapters/definition.tex \
	chapters/instance.tex \
	chapters/value.tex \
	chapters/sequential.tex \
	chapters/common.tex

# need GNU make for the following:
# automatically generated dependencies in this (built) dir
-include objfmt.depend

