# "dox/prsim/Makefile.am"
#	$Id: Makefile.am,v 1.6.18.1 2008/03/16 06:56:35 fang Exp $

include $(top_srcdir)/dox/Make.dox-head

PDFS = hacprsim.pdf
PSS = $(PDFS:.pdf=.ps)
.NOTPARALLEL: hacprsim.dvi hacprsim.pdf

info_TEXINFOS += hacprsim.texi
INFOS = $(TEXINFOS:.texi=.info)

# html_DATA = hacprsim.html
HTML_HOOKS += hacprsim.html

hacprsim_INCLUDES = \
	intro.texi \
	usage.texi \
	commands.texi \
	execution.texi

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

$(srcdir)/options.mk: Makefile.in
$(srcdir)/commands.mk: Makefile.in

-include $(srcdir)/options.mk
-include $(srcdir)/commands.mk
AUTO_TEXINFO_DEPS += options.mk commands.mk

$(HACPRSIM_OPTIONS_TEXI): $(top_srcdir)/src/main/prsim.cc
	$(EXTRACT_TEXINFO_SRCDIR_BATCH_SCRIPT)

$(HACPRSIM_COMMANDS_TEXI): $(top_srcdir)/src/sim/prsim/Command-prsim.cc
	$(EXTRACT_TEXINFO_SRCDIR_BATCH_SCRIPT)

auto_hacprsim_info_DEPS = \
	$(HACPRSIM_OPTIONS_TEXI) \
	$(HACPRSIM_COMMANDS_TEXI)

hacprsim_info_DEPS = \
	$(hacprsim_INCLUDES) \
	$(auto_hacprsim_info_DEPS)

TEXI_CLEANFILES += $(auto_hacprsim_info_DEPS)

EXTRA_DIST += $(hacprsim_info_DEPS)

# extra static dependencies
hacprsim.dvi hacprsim.pdf hacprsim.html $(srcdir)/hacprsim.info: \
	$(hacprsim_info_DEPS)

include $(top_srcdir)/dox/Make.dox-tail

