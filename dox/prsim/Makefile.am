# "dox/prsim/Makefile.am"
#	$Id: Makefile.am,v 1.11.12.1 2010/02/10 06:42:54 fang Exp $

include $(top_srcdir)/dox/Make.dox-head

PDFS = hacprsim.pdf
DVIS = $(PDFS:.pdf=.dvi)
PSS = $(PDFS:.pdf=.ps)
.NOTPARALLEL: hacprsim.dvi hacprsim.pdf

info_TEXINFOS += hacprsim.texi
INFOS = $(TEXINFOS:.texi=.info)

AM_MAKEINFOFLAGS = -I$(srcdir)/../common -I../common
hacmacros.texi: $(srcdir)/../common/hacmacros.texi
	$(LN_SF) $< $@

CLEANFILES += hacmacros.texi

# html_DATA = hacprsim.html
HTML_HOOKS += hacprsim.html

hacprsim_INCLUDES = \
	hacmacros.texi \
	intro.texi \
	usage.texi \
	commands.texi \
	execution.texi \
	diagnostics.texi \
	tips.texi \
	cosim.texi

$(srcdir)/options.mk: usage.texi
	{ $(PRINTF) "HACPRSIM_OPTIONS_TEXI = " ; \
	$(CAT) $< | $(EXTRACT_TEXI_INCLUDES) ; \
	$(ECHO) "" ; \
	$(ECHO) "" ;} > $@

$(srcdir)/commands.mk: commands.texi
	{ $(PRINTF) "HACPRSIM_COMMANDS_TEXI = " ; \
	$(CAT) $< | $(EXTRACT_TEXI_INCLUDES) ; \
	$(ECHO) "" ; \
	$(ECHO) "" ;} > $@

$(srcdir)/vpifuncs.mk: cosim.texi
	{ $(PRINTF) "VPI_FUNCTIONS_TEXI = " ; \
	$(CAT) $< | $(EXTRACT_TEXI_INCLUDES) ; \
	$(ECHO) "" ; \
	$(ECHO) "" ;} > $@

$(srcdir)/options.mk: Makefile.in
$(srcdir)/commands.mk: Makefile.in
$(srcdir)/vpifuncs.mk: Makefile.in

-include $(srcdir)/options.mk
-include $(srcdir)/commands.mk
-include $(srcdir)/vpifuncs.mk
AUTO_TEXINFO_DEPS += options.mk commands.mk vpifuncs.mk

$(HACPRSIM_OPTIONS_TEXI): $(top_srcdir)/src/main/prsim.cc
	$(EXTRACT_TEXINFO_SRCDIR_BATCH_SCRIPT)

$(HACPRSIM_COMMANDS_TEXI): $(top_srcdir)/src/sim/prsim/Command-prsim.cc
	$(EXTRACT_TEXINFO_SRCDIR_BATCH_SCRIPT)

$(VPI_FUNCTIONS_TEXI): $(top_srcdir)/src/sim/prsim/vpi-prsim.cc
	$(EXTRACT_TEXINFO_SRCDIR_BATCH_SCRIPT)


auto_hacprsim_info_DEPS = \
	$(HACPRSIM_OPTIONS_TEXI) \
	$(HACPRSIM_COMMANDS_TEXI) \
	$(VPI_FUNCTIONS_TEXI)

hacprsim_info_DEPS = \
	$(hacprsim_INCLUDES) \
	$(auto_hacprsim_info_DEPS)

TEXI_CLEANFILES += $(auto_hacprsim_info_DEPS)

EXTRA_DIST += $(hacprsim_info_DEPS)

# extra static dependencies
hacprsim.dvi hacprsim.pdf hacprsim.html $(srcdir)/hacprsim.info: \
	$(hacprsim_info_DEPS)

include $(top_srcdir)/dox/Make.dox-tail

