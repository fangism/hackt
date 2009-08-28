# "dox/net/Makefile.am"
#	$Id: Makefile.am,v 1.1.2.1 2009/08/28 00:23:26 fang Exp $

include $(top_srcdir)/dox/Make.dox-head

PDFS = hacknet.pdf
DVIS = $(PDFS:.pdf=.dvi)
PSS = $(PDFS:.pdf=.ps)
.NOTPARALLEL: hacknet.dvi hacknet.pdf

################### texinfo documentation #####################################

info_TEXINFOS += hacknet.texi
INFOS = $(TEXINFOS:.texi=.info)

AM_MAKEINFOFLAGS = -I$(srcdir)/../common -I../common
hacmacros.texi: $(srcdir)/../common/hacmacros.texi
	$(LN_S) $< $@

CLEANFILES += hacmacros.texi

# install doesn't know how to handle generated html *directory* :(
# html_DATA = hacknet.html
HTML_HOOKS += hacknet.html

hacknet_INCLUDES = \
	hacmacros.texi \
	intro.texi \
	usage.texi

# HACKNET_OPTIONS_TEXI = ...
# HACKNET_CONFIG_TEXI = ...
# source-extracted texinfo files: (can we deduce this automatically? done.)
$(srcdir)/options.mk: usage.texi
	{ $(PRINTF) "HACKNET_OPTIONS_TEXI = " ; \
	$(CAT) $< | $(EXTRACT_TEXI_INCLUDES) ; \
	$(ECHO) "" ; \
	$(ECHO) "" ;} > $@
$(srcdir)/config.mk: config.texi
	{ $(PRINTF) "HACKNET_CONFIG_TEXI = " ; \
	$(CAT) $< | $(EXTRACT_TEXI_INCLUDES) ; \
	$(ECHO) "" ; \
	$(ECHO) "" ;} > $@

# These are typically only built by the maintainer, since they are distributed.
# Don't depend on Makefile, because that is configure-generated,
# which would cause the following to be rebuilt by the user (not desired).
$(srcdir)/options.mk: Makefile.in
$(srcdir)/config.mk: Makefile.in

-include $(srcdir)/options.mk
-include $(srcdir)/config.mk
AUTO_TEXINFO_DEPS += options.mk config.mk
# these could be CVS cleaned too...

# NOTE: these auto-generated .texi files are emitted in the maintainer's
# $(srcdir), however, they are distributed as part of $(srcdir).  
$(HACKNET_OPTIONS_TEXI): $(top_srcdir)/src/main/hacknet.cc
	$(EXTRACT_TEXINFO_SRCDIR_BATCH_SCRIPT)
$(HACKNET_CONFIG_TEXI): $(top_srcdir)/src/net/netgraph.cc
	$(EXTRACT_TEXINFO_SRCDIR_BATCH_SCRIPT)


# just re-generate all of them when any one changes...
# NOTE: this is a many-to-many dependence, which means that
# there are some false dependencies which may not be resolvable.
# If you find make keeps re-running texinfo programs, run "make clean-texi"
# once first to force-regenerate all dependent texis.  

auto_hacknet_info_DEPS = \
	$(HACKNET_OPTIONS_TEXI) \
	$(HACKNET_CONFIG_TEXI)

hacknet_info_DEPS = \
	$(hacknet_INCLUDES) \
	$(auto_hacknet_info_DEPS)

# these auto-generated and distributed files should not be cleaned by the user
# they should only be explicitly cleaned by the maintainer.  
TEXI_CLEANFILES += $(auto_hacknet_info_DEPS)

# For a srcdir target like the .info to be distributed, 
# its dependencies are best distributed along with to keep 
# the user's build from trying to regenerate the .info.
# This can occur during a distcheck if auto-generated .texi dependencies
# are not shipped with the distribution.  
EXTRA_DIST += $(hacknet_info_DEPS)

# not bothering with more sophisticated dependency tracking
hacknet.dvi hacknet.pdf hacknet.html $(srcdir)/hacknet.info: \
	$(hacknet_info_DEPS)


################# documentation figures #######################################
# SOURCE_FIGS =
# SOURCE_DOTS =
# BUILT_FIGS = $(SOURCE_DOTS:.dot=.fig)
# XFIG_FIGURES = $(SOURCE_FIGS) $(BUILT_FIGS)
# XFIG_TEX = $(XFIG_FIGURES:.fig=.figtex)
# XFIG_PSTEX = $(XFIG_FIGURES:.fig=.ps)
# XFIG_PSTEX_T = $(XFIG_FIGURES:.fig=.pstex_t)
# XFIG_PDFTEX = $(XFIG_FIGURES:.fig=.pdf)
# XFIG_PDFTEX_T = $(XFIG_FIGURES:.fig=.pdftex_t)
# XFIG_EPS = $(XFIG_FIGURES:.fig=.eps)
# XFIG_PNG = $(XFIG_FIGURES:.fig=.png)

all-figs: $(BUILT_FIGS)

# distribute to remove burden from user to build dox
EXTRA_DIST += $(XFIG_FIGURES) $(SOURCE_DOTS) $(XFIG_PNG) \
	$(XFIG_EPS) $(XFIG_PDFTEX)
# PNGs and built .figs too because distributed .info depends on them
# (don't want user to have to rebuild .info)

CLEANFILES += $(BUILT_FIGS)

# COMMON_DEPS = $(srcdir)/../common/hac.bib $(XFIG_TEX)

# all-figs named so automake doesn't think we're overriding the targets' rule
# LIMITATION: choosing not to try to include the .pdftex_t for now
# built-figs hacknet.dvi: $(XFIG_EPS)
# built-figs hacknet.pdf hacknet.html: $(XFIG_PDFTEX)
# built-figs hacknet.html: $(XFIG_PNG)
# built-figs $(srcdir)/hacknet.info: $(XFIG_PNG)
# still not sure why html looks for .eps figures...

# .PHONY: built-figs


$(srcdir)/figs/.cvsignore: $(srcdir)/.cvsignore
	$(CP) $< $@

include $(top_srcdir)/dox/Make.dox-tail

