# "dox/tutorial/Makefile.am"
# Makefile for tutorial directory
#	$Id: Makefile.am,v 1.14 2008/03/21 00:19:51 fang Exp $

include $(top_srcdir)/dox/Make.dox-head

PDFS = hackt.pdf
PSS = $(PDFS:.pdf=.ps)
.NOTPARALLEL: hackt.dvi hackt.pdf

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

$(srcdir)/compile.mk: hackt/compiler.texi
	{ $(PRINTF) "COMPILE_OPTIONS_TEXI = " ; \
	$(CAT) $< | $(EXTRACT_TEXI_INCLUDES) ; \
	$(ECHO) "" ; \
	$(ECHO) "" ;} > $@

$(srcdir)/cflat.mk: hackt/legacy.texi
	{ $(PRINTF) "HFLAT_OPTIONS_TEXI = " ; \
	$(CAT) $< | $(EXTRACT_TEXI_INCLUDES) ; \
	$(ECHO) "" ; \
	$(ECHO) "" ;} > $@

$(srcdir)/cflat.mk: Makefile.in
$(srcdir)/compile.mk: Makefile.in

-include $(srcdir)/cflat.mk
AUTO_TEXINFO_DEPS += cflat.mk

-include $(srcdir)/compile.mk
AUTO_TEXINFO_DEPS += compile.mk

$(COMPILE_OPTIONS_TEXI): \
		$(top_srcdir)/src/main/compile.cc \
		$(top_srcdir)/src/main/main_funcs.cc 
	$(EXTRACT_TEXINFO_SRCDIR_BATCH_SCRIPT)

$(HFLAT_OPTIONS_TEXI): $(top_srcdir)/src/main/cflat.cc
	$(EXTRACT_TEXINFO_SRCDIR_BATCH_SCRIPT)

auto_hackt_info_DEPS = \
	$(COMPILE_OPTIONS_TEXI) \
	$(HFLAT_OPTIONS_TEXI)
hackt_info_DEPS = $(hackt_INCLUDES) $(auto_hackt_info_DEPS)

TEXI_CLEANFILES += $(auto_hackt_info_DEPS)

EXTRA_DIST += $(auto_hackt_info_DEPS)

hackt.dvi hackt.pdf hackt.html $(srcdir)/hackt.info: $(hackt_info_DEPS)

EXTRA_DIST += README ChangeLog

include $(top_srcdir)/dox/Make.dox-tail

