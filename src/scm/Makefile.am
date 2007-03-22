# "src/scm/Makefile.am"
#	$Id: Makefile.am,v 1.1.2.2 2007/03/22 05:17:49 fang Exp $

include $(top_srcdir)/Make.stddef

scmdir = $(pkgdatadir)/scm

# install as $(pkgdatadir)/scm/hackt/...
nobase_dist_scm_DATA = \
	hackt/streams.scm \
	hackt/chpsim-trace.scm

EXTRA_DIST += README \
	ChangeLog \
	TODO

# just re-use parent srcdir's template
local.cvsignore: $(srcdir)/../local.cvsignore
	$(LN_S) $< $@

include $(top_srcdir)/Make.global

