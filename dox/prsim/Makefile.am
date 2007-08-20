# "dox/prsim/Makefile.am"
#	$Id: Makefile.am,v 1.3 2007/08/20 21:12:30 fang Exp $

include $(top_srcdir)/dox/Make.dox-head

PDFS = hacprsim.pdf
PSS = $(PDFS:.pdf=.ps)

# deprecated
noinst_DATA += prsim.pdf prsim.ps
.NOTPARALLEL: prsim.pdf prsim.dvi

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
	{ $(ECHO) -n "HACPRSIM_OPTIONS_TEXI = " ; \
	$(CAT) $< | $(EXTRACT_TEXI_INCLUDES) ; \
	$(ECHO) "" ; \
	$(ECHO) "" ;} > $@

$(srcdir)/commands.mk: commands.texi
	{ $(ECHO) -n "HACPRSIM_COMMANDS_TEXI = " ; \
	$(CAT) $< | $(EXTRACT_TEXI_INCLUDES) ; \
	$(ECHO) "" ; \
	$(ECHO) "" ;} > $@

$(srcdir)/options.mk: Makefile.in
$(srcdir)/commands.mk: Makefile.in

-include $(srcdir)/options.mk
-include $(srcdir)/commands.mk

AUTO_TEXINFO_DEPS = options.mk commands.mk
EXTRA_DIST += $(AUTO_TEXINFO_DEPS)
AUTO_IGNORE += $(AUTO_TEXINFO_DEPS)
MAINTAINERCLEANFILES = $(AUTO_TEXINFO_DEPS)

$(HACPRSIM_OPTIONS_TEXI): $(top_srcdir)/src/main/prsim.cc
	$(EXTRACT_TEXINFO_SRCDIR_BATCH_SCRIPT)

$(HACPRSIM_COMMANDS_TEXI): $(top_srcdir)/src/sim/prsim/Command-prsim.cc
	$(EXTRACT_TEXINFO_SRCDIR_BATCH_SCRIPT)

hacprsim_info_DEPS = \
	$(hacprsim_INCLUDES) \
	$(HACPRSIM_OPTIONS_TEXI) \
	$(HACPRSIM_COMMANDS_TEXI)

MAINTAINERCLEANFILES += \
	$(HACPRSIM_OPTIONS_TEXI) \
	$(HACPRSIM_COMMANDS_TEXI)

# manually clean (intended for maintainer)
clean-texi:
	-$(RM) $(HACPRSIM_OPTIONS_TEXI)
	-$(RM) $(HACPRSIM_COMMANDS_TEXI)

.PHONY: clean-texi

EXTRA_DIST += $(hacprsim_info_DEPS)

# extra static dependencies
hacprsim.dvi hacprsim.pdf hacprsim.html $(srcdir)/hacprsim.info: \
	$(hacprsim_info_DEPS)

########### old latex targets #################################################
# have to add these by hand? :S
EXTRA_DIST += \
	prsim.tex

# need GNU make for the following:
# automatically generated dependencies in this (built) dir
# -include cast2hac.depend
# AUTO_DEPENDS += cast2hac.depend
-include prsim.pdfdepend
-include prsim.dvidepend
AUTO_DEPENDS += prsim.pdfdepend prsim.dvidepend

include $(top_srcdir)/dox/Make.dox-tail

