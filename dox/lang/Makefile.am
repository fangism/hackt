# "dox/lang/Makefile.am"
#	$Id: Makefile.am,v 1.14 2005/05/04 19:31:42 fang Exp $

include $(top_srcdir)/dox/mk/Make.latex

PDFS = artxx.pdf
PSS = $(PDFS:.pdf=.ps)

# all-local: pdf-local ps-local

pdf-local: $(PDFS)

ps-local: $(PSS)

# have to add these by hand? :S
EXTRA_DIST = \
	artxx.bib artxx.tex \
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
-include artxx.depend

