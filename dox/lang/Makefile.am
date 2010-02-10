# "dox/lang/Makefile.am"
#	$Id: Makefile.am,v 1.35.4.1 2010/02/10 06:42:53 fang Exp $

include $(top_srcdir)/dox/Make.dox-head

PDFS = hac.pdf
DVIS = $(PDFS:.pdf=.dvi)
PSS = $(PDFS:.pdf=.ps)
.NOTPARALLEL: hac.pdf hac.dvi

info_TEXINFOS += hac.texi
INFOS = $(TEXINFOS:.texi=.info)
# html_DATA += hac.html
HTML_HOOKS += hac.html

AM_MAKEINFOFLAGS = -I$(srcdir)/../common -I../common

# generate grammar appendix
GRAMMAR_DIR = $(top_builddir)/src
GRAMMAR_OUTPUT = parser/hackt-parse.output
GRAMMAR_SOURCE = $(GRAMMAR_DIR)/$(GRAMMAR_OUTPUT)
GRAMMAR_TEXI = grammar/rules.texi

$(GRAMMAR_SOURCE):
	cd $(GRAMMAR_DIR) && $(MAKE) $(AM_MAKEFLAGS) $(GRAMMAR_OUTPUT)

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
	chapters/attributes.texi \
	chapters/typedefs.texi \
	chapters/linkage.texi \
	chapters/CHP.texi \
	chapters/PRS.texi \
	chapters/SPEC.texi \
	grammar/keywords.texi \
	grammar/grammar.texi \
	$(srcdir)/$(GRAMMAR_TEXI)

hacmacros.texi: $(srcdir)/../common/hacmacros.texi
	$(LN_SF) $< $@

CLEANFILES += hacmacros.texi

$(srcdir)/attribs.mk: chapters/attributes.texi
	{ $(PRINTF) "INSTANCE_ATTRIBUTES_TEXI = " ; \
	$(CAT) $< | $(EXTRACT_TEXI_INCLUDES) ; \
	$(ECHO) "" ; \
	$(ECHO) "" ;} > $@

$(srcdir)/prsmacros.mk: chapters/PRS.texi
	{ $(PRINTF) "PRS_MACROS_TEXI = " ; \
	$(CAT) $< | $(EXTRACT_TEXI_INCLUDES) ; \
	$(ECHO) "" ; \
	$(ECHO) "" ;} > $@

$(srcdir)/spec.mk: chapters/SPEC.texi
	{ $(PRINTF) "SPEC_DIRECTIVES_TEXI = " ; \
	$(CAT) $< | $(EXTRACT_TEXI_INCLUDES) ; \
	$(ECHO) "" ; \
	$(ECHO) "" ;} > $@

$(srcdir)/attribs.mk: Makefile.in
$(srcdir)/prsmacros.mk: Makefile.in
$(srcdir)/spec.mk: Makefile.in

-include $(srcdir)/attribs.mk
-include $(srcdir)/prsmacros.mk
-include $(srcdir)/spec.mk

AUTO_TEXINFO_DEPS += attribs.mk prsmacros.mk spec.mk

$(INSTANCE_ATTRIBUTES_TEXI): \
		$(top_srcdir)/src/Object/unroll/bool_instance_attribute.cc \
		$(top_srcdir)/src/Object/unroll/channel_instance_attribute.cc \
		$(top_srcdir)/src/Object/unroll/int_instance_attribute.cc \
		$(top_srcdir)/src/Object/unroll/enum_instance_attribute.cc \
		$(top_srcdir)/src/Object/unroll/process_instance_attribute.cc
	$(EXTRACT_TEXINFO_SRCDIR_BATCH_SCRIPT)

$(PRS_MACROS_TEXI): \
		$(top_srcdir)/src/Object/lang/PRS_macro_registry.cc \
		$(top_srcdir)/src/Object/lang/PRS_attribute_registry.cc \
		$(top_srcdir)/src/Object/lang/PRS_literal_attribute_registry.cc
	$(EXTRACT_TEXINFO_SRCDIR_BATCH_SCRIPT)

$(SPEC_DIRECTIVES_TEXI): $(top_srcdir)/src/Object/lang/SPEC_registry.cc
	$(EXTRACT_TEXINFO_SRCDIR_BATCH_SCRIPT)

# don't make true dependency becaues srcdir depending on built file
# will break distribution, forcing docs to always be rebuilt.
# $(srcdir)/$(GRAMMAR_TEXI): $(GRAMMAR_SOURCE)
$(srcdir)/$(GRAMMAR_TEXI):
	test -f $(GRAMMAR_SOURCE) && \
	$(CAT) $(GRAMMAR_SOURCE) | \
	$(AWK) '{if (match($$1,"[0-9]+") && match($$0,"[:|]") && !match($$0,"[.]")) print;}' | \
	  $(SED) -e 's/@/@@/g' -e 's/{/@{/g' -e 's/}/@}/g' > $@
# the filter should work for yacc and bison
# sed filtering is to escape braces for non-verbatim texinfo inclusion

hac_info_DEPS = \
	$(hac_info_INCLUDES) \
	$(INSTANCE_ATTRIBUTES_TEXI) \
	$(PRS_MACROS_TEXI) \
	$(SPEC_DIRECTIVES_TEXI)

TEXI_CLEANFILES += \
	$(INSTANCE_ATTRIBUTES_TEXI) \
	$(PRS_MACROS_TEXI) \
	$(SPEC_DIRECTIVES_TEXI) \
	$(srcdir)/$(GRAMMAR_TEXI)

hac.pdf hac.dvi hac.html $(srcdir)/hac.info: $(hac_info_DEPS)

EXTRA_DIST += $(hac_info_DEPS)

###############################################################################
EXTRA_DIST += \
	TODO ChangeLog

include $(top_srcdir)/dox/Make.dox-tail

