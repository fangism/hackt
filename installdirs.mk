# "installdirs.mk
#	$Id: installdirs.mk,v 1.3 2009/02/23 09:11:10 fang Exp $
# install paths that depend on variables defined in installcheck.mk
# Rationale for this file:
# * Minimize definition clutter in Make.stddef
# * Define common install subdirectories globally with minimal
#	configure-dependency, these are configure-independent relative dirs

# Prerequisite:
#include $(top_builddir)/installcheck.mk
# or equivalent definitions, which define configure-dependent dirs

# this path should be consistent with scripts/init_hackt.[c]sh
# also keep consistent with hackt-config.in
scmdir = $(pkgdatadir)/scm

# more install directories
# NOTE: these variables may already be defined by autoconf-2.60+
# where various documents (pdf, ps) should be installed
# Keep these consistent with src/scripts/init_hackt.{csh,sh}[.in}
docdir = $(pkgdatadir)/doc
# infodir = $(datadir)/info
htmldir = $(docdir)/html
pdfdir = $(docdir)/pdf
psdir = $(docdir)/ps
# using arch-specific directories because they reference exec_prefix paths
examplesdir = $(docdir)/examples/$(host)

