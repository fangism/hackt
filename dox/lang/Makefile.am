# "dox/lang/Makefile.am"
#	$Id: Makefile.am,v 1.22 2007/01/24 04:14:49 fang Exp $

PDFS = hac.pdf
PSS = $(PDFS:.pdf=.ps)

include $(top_srcdir)/dox/Make.dox-head

# have to add these by hand? :S
EXTRA_DIST += \
	hac.tex \
	chapters/preface.tex \
	chapters/goals.tex \
	chapters/intro.tex \
	chapters/types.tex \
	chapters/expressions.tex \
	chapters/arrays.tex \
	chapters/processes.tex \
	chapters/channels.tex \
	chapters/datatypes.tex \
	chapters/namespaces.tex \
	chapters/templates.tex \
	chapters/typedefs.tex \
	chapters/connections.tex \
	chapters/linkage.tex \
	chapters/CHP.tex \
	chapters/PRS.tex \
	chapters/SPEC.tex \
	TODO ChangeLog

# need GNU make for the following:
# automatically generated dependencies in this (built) dir
# -include hac.depend
# AUTO_DEPENDS += hac.depend 
-include hac.pdfdepend
-include hac.dvidepend
AUTO_DEPENDS += hac.pdfdepend hac.dvidepend

hac.pdf hac.dvi: $(srcdir)/../common/hac.bib

include $(top_srcdir)/dox/Make.dox-tail

