# "dox/lang/Makefile.am"
#	$Id: Makefile.am,v 1.27 2007/09/13 23:52:58 fang Exp $

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

# generate grammar appendix
GRAMMAR_SOURCE = $(top_builddir)/src/parser/hackt-parse.output
GRAMMAR_TEXI = grammar/rules.texi

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
	chapters/SPEC.texi \
	grammar/keywords.texi \
	grammar/grammar.texi \
	$(srcdir)/$(GRAMMAR_TEXI)

hacmacros.texi: $(srcdir)/../common/hacmacros.texi
	$(LN_S) $< $@

CLEANFILES += hacmacros.texi

$(srcdir)/prsmacros.mk: chapters/PRS.texi
	{ $(ECHO) -n "PRS_MACROS_TEXI = " ; \
	$(CAT) $< | $(EXTRACT_TEXI_INCLUDES) ; \
	$(ECHO) "" ; \
	$(ECHO) "" ;} > $@

$(srcdir)/spec.mk: chapters/SPEC.texi
	{ $(ECHO) -n "SPEC_DIRECTIVES_TEXI = " ; \
	$(CAT) $< | $(EXTRACT_TEXI_INCLUDES) ; \
	$(ECHO) "" ; \
	$(ECHO) "" ;} > $@

$(srcdir)/prsmacros.mk: Makefile.in
$(srcdir)/spec.mk: Makefile.in

-include $(srcdir)/prsmacros.mk
-include $(srcdir)/spec.mk

AUTO_TEXINFO_DEPS += prsmacros.mk spec.mk

$(PRS_MACROS_TEXI): \
		$(top_srcdir)/src/Object/lang/PRS_macro_registry.cc \
		$(top_srcdir)/src/Object/lang/PRS_attribute_registry.cc
	$(EXTRACT_TEXINFO_SRCDIR_BATCH_SCRIPT)

$(SPEC_DIRECTIVES_TEXI): $(top_srcdir)/src/Object/lang/SPEC_registry.cc
	$(EXTRACT_TEXINFO_SRCDIR_BATCH_SCRIPT)

# don't make true dependency becaues srcdir depending on built file
# will break distribution, forcing docs to always be rebuilt.
# $(srcdir)/$(GRAMMAR_TEXI).texi: $(GRAMMAR_SOURCE)
$(srcdir)/$(GRAMMAR_TEXI):
	test -f $(GRAMMAR_SOURCE) && \
	$(CAT) $(GRAMMAR_SOURCE) | \
	$(AWK) '{if (match($$1,"[0-9]+") && match($$0,"[:|]") && !match($$0,"[.]")) print;}' | \
	  $(SED) -e 's/{/@{/g' -e 's/}/@}/g' > $@
# the filter should work for yacc and bison
# sed filtering is to escape braces for non-verbatim texinfo inclusion

hac_info_DEPS = \
	$(hac_info_INCLUDES) \
	$(PRS_MACROS_TEXI) \
	$(SPEC_DIRECTIVES_TEXI)

TEXI_CLEANFILES += \
	$(PRS_MACROS_TEXI) \
	$(SPEC_DIRECTIVES_TEXI) \
	$(srcdir)/$(GRAMMAR_TEXI)

hac.pdf hac.dvi hac.html $(srcdir)/hac.info: $(hac_info_DEPS)

EXTRA_DIST += $(hac_info_DEPS)

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

