# "dox/cast2hac/Makefile.am"
#	$Id: Makefile.am,v 1.16.12.1 2010/02/10 06:42:51 fang Exp $

include $(top_srcdir)/dox/Make.dox-head

PDFS = cast2hac.pdf
DVIS = $(PDFS:.pdf=.dvi)
PSS = $(PDFS:.pdf=.ps)
.NOTPARALLEL: cast2hac.dvi cast2hac.pdf

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
	expressions.texi \
	loops.texi \
	templates.texi \
	typedefs.texi \
	PRS.texi \
	CHP.texi \
	spec.texi \
	cflat.texi

hacmacros.texi: $(srcdir)/../common/hacmacros.texi
	$(LN_SF) $< $@

CLEANFILES += hacmacros.texi

# without $(srcdir) since makeinfo is run in the srcdir
AM_MAKEINFOFLAGS = -I ../common -I $(srcdir)/../common

cast2hac.pdf cast2hac.dvi cast2hac.html $(srcdir)/cast2hac.info: \
	$(cast2hac_info_INCLUDES)

EXTRA_DIST += $(cast2hac_info_INCLUDES)

# install doesn't know how to handle generated html *directory* :(
# html_DATA = cast2hac.html
HTML_HOOKS += cast2hac.html

EXTRA_DIST += ChangeLog 

include $(top_srcdir)/dox/Make.dox-tail

