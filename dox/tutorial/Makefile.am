# "dox/tutorial/Makefile.am"
# Makefile for tutorial directory
#	$Id: Makefile.am,v 1.8 2007/08/09 03:47:10 fang Exp $

PDFS = hackt.pdf
PSS = $(PDFS:.pdf=.ps)

.NOTPARALLEL: hackt.pdf hackt.dvi

include $(top_srcdir)/dox/Make.dox-head

EXTRA_DIST += README ChangeLog \
	hackt.tex \
	hackt/intro.tex \
	hackt/compiler.tex \
	hackt/shell.tex \
	hackt/diagnostics.tex \
	hackt/legacy.tex

# -include hackt.depend
# AUTO_DEPENDS += hackt.depend
-include hackt.pdfdepend
-include hackt.dvidepend
AUTO_DEPENDS += hackt.pdfdepend hackt.dvidepend

include $(top_srcdir)/dox/Make.dox-tail

