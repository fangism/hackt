# "dox/util/Makefile.am"
#	$Id: Makefile.am,v 1.1 2005/05/04 19:31:44 fang Exp $

include $(top_srcdir)/dox/mk/Make.latex

PDFS = fangutil.pdf
PSS = $(PDFS:.pdf=.ps)

# all-local: pdf-local ps-local

pdf-local: $(PDFS)

ps-local: $(PSS)

# have to add these by hand? :S
EXTRA_DIST = fangutil.tex \
	chapters/pointers.tex \
	chapters/persistent.tex

# need GNU make for the following:
# automatically generated dependencies in this (built) dir
-include fangutil.depend

