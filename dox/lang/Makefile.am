# "dox/lang/Makefile.am"
#	$Id: Makefile.am,v 1.15.32.1 2005/12/13 03:58:21 fang Exp $

include $(top_srcdir)/dox/Make.dox-head

PDFS = hac.pdf
PSS = $(PDFS:.pdf=.ps)

# all-local: pdf-local ps-local

pdf-local: $(PDFS)

ps-local: $(PSS)

# have to add these by hand? :S
EXTRA_DIST += \
	hac.bib hac.tex \
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
	TODO ChangeLog

# need GNU make for the following:
# automatically generated dependencies in this (built) dir
-include hac.depend

include $(top_srcdir)/dox/Make.dox-tail

