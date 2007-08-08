# "installdirs.mk[.in]
#	$Id: installdirs.mk,v 1.1 2007/08/08 23:01:25 fang Exp $
# install paths that depend on variables defined in installcheck.mk
# Rationale for this file:
# * Minimize definition clutter in Make.stddef
# * Define common install subdirectories globally with minimal
#	configure-dependency

# Prerequisite:
#include $(top_builddir)/installcheck.mk
# or equivalent definitions

# more install directories
# NOTE: these variables may already be defined by autoconf-2.60+
# where various documents (pdf, ps) should be installed
# Keep these consistent with src/scripts/init_hackt.{csh,sh}[.in}
docdir = $(pkgdatadir)/doc
# infodir = $(datadir)/info
htmldir = $(docdir)/html
pdfdir = $(docdir)/pdf
psdir = $(docdir)/ps
examplesdir = $(docdir)/examples

