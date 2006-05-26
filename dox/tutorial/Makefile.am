# "dox/tutorial/Makefile.am"
# Makefile for tutorial directory
#	$Id: Makefile.am,v 1.4 2006/05/26 07:26:59 fang Exp $

PDFS = hackt.pdf
PSS = $(PDFS:.pdf=.ps)

include $(top_srcdir)/dox/Make.dox-head

EXTRA_DIST += README ChangeLog \
	hackt/intro.tex \
	hackt/compiler.tex \
	hackt/shell.tex \
	hackt/diagnostics.tex \
	hackt/legacy.tex

-include hackt.depend

include $(top_srcdir)/dox/Make.dox-tail

