# "dox/lang/Makefile.am"
#	$Id: Makefile.am,v 1.24 2007/08/20 00:02:35 fang Exp $

include $(top_srcdir)/dox/Make.dox-head

PDFS = hac.pdf
PSS = $(PDFS:.pdf=.ps)

noinst_DATA += hac-tex.pdf hac-tex.dvi
.NOTPARALLEL: hac-tex.pdf hac-tex.dvi

info_TEXINFOS += hac.texi
INFOS = $(TEXINFOS:.texi=.info)
# html_DATA += hac.html
HTML_HOOKS += hac.html

AM_MAKEINFOFLAGS = -I$(srcdir)/../common -I../common

hac_info_INCLUDES = \
	hacmacros.texi \
	chapters/preface.texi \
	chapters/goals.texi \
	chapters/intro.texi \
	chapters/types.texi \
	chapters/expressions.texi \
	chapters/arrays.texi \
	chapters/processes.texi \
	chapters/channels.texi \
	chapters/datatypes.texi \
	chapters/namespaces.texi \
	chapters/templates.texi \
	chapters/connections.texi \
	chapters/typedefs.texi \
	chapters/linkage.texi \
	chapters/CHP.texi \
	chapters/PRS.texi \
	chapters/SPEC.texi

hacmacros.texi: $(srcdir)/../common/hacmacros.texi
	$(LN_S) $< $@

CLEANFILES += hacmacros.texi

hac.pdf hac.dvi hac.html $(srcdir)/hac.info: $(hac_info_INCLUDES)

EXTRA_DIST += $(hac_info_INCLUDES)

###############################################################################
# obsolete latex sources
# have to add these by hand? :S
EXTRA_DIST += \
	hac-tex.tex \
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
-include hac-tex.pdfdepend
-include hac-tex.dvidepend
AUTO_DEPENDS += hac-tex.pdfdepend hac-tex.dvidepend

hac-tex.pdf hac-tex.dvi: $(srcdir)/../common/hac.bib

include $(top_srcdir)/dox/Make.dox-tail

