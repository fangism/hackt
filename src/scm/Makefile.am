# "src/scm/Makefile.am"
#	$Id: Makefile.am,v 1.1.2.3 2007/03/24 03:30:39 fang Exp $

include $(top_srcdir)/Make.stddef

scmdir = $(pkgdatadir)/scm

# install as $(pkgdatadir)/scm/hackt/...
nobase_dist_scm_DATA = \
	hackt/algorithm.scm \
	hackt/streams.scm \
	hackt/hackt.scm \
	hackt/chpsim-trace.scm

EXTRA_DIST += README \
	ChangeLog \
	TODO

# just re-use parent srcdir's template
local.cvsignore: $(srcdir)/../local.cvsignore
	$(LN_S) $< $@

include $(top_srcdir)/Make.global

