# "dox/cast2hac/Makefile.am"
#	$Id: Makefile.am,v 1.11 2007/08/16 23:48:04 fang Exp $

include $(top_srcdir)/dox/Make.dox-head

PDFS = cast2hac.pdf
PSS = $(PDFS:.pdf=.ps)
.NOTPARALLEL: cast2hac-tex.pdf cast2hac-tex.dvi

noinst_DATA += cast2hac-tex.pdf cast2hac-tex.dvi

info_TEXINFOS += cast2hac.texi
INFOS = $(TEXINFOS:.texi=.info)

cast2hac_info_INCLUDES = \
	hacmacros.texi \
	intro.texi \
	types.texi \
	definitions.texi \
	instances.texi \
	arrays.texi \
	connections.texi \
	loops.texi \
	templates.texi \
	typedefs.texi \
	PRS.texi \
	CHP.texi \
	spec.texi \
	cflat.texi

hacmacros.texi: $(srcdir)/../common/hacmacros.texi
	$(LN_S) $< $@

CLEANFILES += hacmacros.texi

AM_MAKEINFOFLAGS = -I $(srcdir)/../common

cast2hac.pdf cast2hac.dvi cast2hac.html $(srcdir)/cast2hac.info: \
	$(cast2hac_info_INCLUDES)

EXTRA_DIST += $(cast2hac_info_INCLUDES)

# install doesn't know how to handle generated html *directory* :(
# html_DATA = cast2hac.html
HTML_HOOKS += cast2hac.html

# have to add these by hand? :S
EXTRA_DIST += \
	ChangeLog \
	cast2hac-tex.tex \
	intro.tex \
	CHP.tex \
	PRS.tex \
	arrays.tex \
	connections.tex \
	definitions.tex \
	instances.tex \
	loops.tex \
	spec.tex \
	templates.tex \
	types.tex \
	typedefs.tex \
	cflat.tex

# need GNU make for the following:
# automatically generated dependencies in this (built) dir
# -include cast2hac.depend
# AUTO_DEPENDS += cast2hac.depend
-include cast2hac-tex.pdfdepend
-include cast2hac-tex.dvidepend
AUTO_DEPENDS += cast2hac-tex.pdfdepend cast2hac-tex.dvidepend

include $(top_srcdir)/dox/Make.dox-tail

