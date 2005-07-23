# "dox/util/Makefile.am"
#	$Id: Makefile.am,v 1.2.16.2 2005/07/23 04:27:55 fang Exp $

include $(top_srcdir)/dox/Make.dox-head

PDFS = fangutil.pdf
PSS = $(PDFS:.pdf=.ps)

# all-local: pdf-local ps-local

pdf-local: $(PDFS)

ps-local: $(PSS)

# have to add these by hand? :S
EXTRA_DIST += fangutil.tex fangutil.bib \
	chapters/pointers.tex \
	chapters/persistent.tex

# need GNU make for the following:
# automatically generated dependencies in this (built) dir
-include fangutil.depend

include $(top_srcdir)/dox/Make.dox-tail

