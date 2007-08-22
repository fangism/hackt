# "dox/tutorial/Makefile.am"
# Makefile for tutorial directory
#	$Id: Makefile.am,v 1.10 2007/08/22 02:25:58 fang Exp $

include $(top_srcdir)/dox/Make.dox-head

PDFS = hackt.pdf
PSS = $(PDFS:.pdf=.ps)

noinst_DATA += hackt.pdf hackt.ps
.NOTPARALLEL: hackt-tex.pdf hackt-tex.dvi

info_TEXINFOS += hackt.texi
INFOS = $(TEXINFOS:.texi=.info)

# html_DATA = hackt.html
HTML_HOOKS += hackt.html

hackt_INCLUDES = \
	hacmacros.texi \
	hackt/intro.texi \
	hackt/compiler.texi \
	hackt/shell.texi \
	hackt/diagnostics.texi \
	hackt/legacy.texi

EXTRA_DIST += $(hackt_INCLUDES)

hacmacros.texi: $(srcdir)/../common/hacmacros.texi
	$(LN_S) $< $@

CLEANFILES += hacmacros.texi

$(srcdir)/cflat.mk: hackt/legacy.texi
	{ $(ECHO) -n "HFLAT_OPTIONS_TEXI = " ; \
	$(CAT) $< | $(EXTRACT_TEXI_INCLUDES) ; \
	$(ECHO) "" ; \
	$(ECHO) "" ;} > $@

$(srcdir)/cflat.mk: Makefile.in

-include $(srcdir)/cflat.mk
AUTO_TEXINFO_DEPS += cflat.mk

$(HFLAT_OPTIONS_TEXI): $(top_srcdir)/src/main/cflat.cc
	$(EXTRACT_TEXINFO_SRCDIR_BATCH_SCRIPT)

auto_hackt_info_DEPS = $(HFLAT_OPTIONS_TEXI)
hackt_info_DEPS = $(hackt_INCLUDES) $(auto_hackt_info_DEPS)

TEXI_CLEANFILES += $(auto_hackt_info_DEPS)

EXTRA_DIST += $(auto_hackt_info_DEPS)

hackt.dvi hackt.pdf hackt.html $(srcdir)/hackt.info: $(hackt_info_DEPS)

######## old latex sources ####################################################
EXTRA_DIST += README ChangeLog \
	hackt-tex.tex \
	hackt/intro.tex \
	hackt/compiler.tex \
	hackt/shell.tex \
	hackt/diagnostics.tex \
	hackt/legacy.tex

# -include hackt.depend
# AUTO_DEPENDS += hackt.depend
-include hackt-tex.pdfdepend
-include hackt-tex.dvidepend
AUTO_DEPENDS += hackt-tex.pdfdepend hackt-tex.dvidepend

include $(top_srcdir)/dox/Make.dox-tail

