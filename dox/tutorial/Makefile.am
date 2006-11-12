# "dox/tutorial/Makefile.am"
# Makefile for tutorial directory
#	$Id: Makefile.am,v 1.6 2006/11/12 05:57:51 fang Exp $

PDFS = hackt.pdf
PSS = $(PDFS:.pdf=.ps)

include $(top_srcdir)/dox/Make.dox-head

EXTRA_DIST += README ChangeLog \
	hackt.tex \
	hackt/intro.tex \
	hackt/compiler.tex \
	hackt/shell.tex \
	hackt/diagnostics.tex \
	hackt/legacy.tex

-include hackt.depend
AUTO_DEPENDS += hackt.depend

include $(top_srcdir)/dox/Make.dox-tail

