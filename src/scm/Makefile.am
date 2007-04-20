# "src/scm/Makefile.am"
#	$Id: Makefile.am,v 1.2 2007/04/20 18:26:02 fang Exp $

include $(top_srcdir)/Make.stddef

# this path should be consistent with scripts/init_hackt.[c]sh
# also keep consistent with hackt-config.in
scmdir = $(pkgdatadir)/scm

# install as $(pkgdatadir)/scm/hackt/...
nobase_dist_scm_DATA = \
	hackt/algorithm.scm \
	hackt/streams.scm \
	hackt/rb-tree.scm \
	hackt/hackt.scm \
	hackt/chpsim.scm \
	hackt/chpsim-trace.scm

EXTRA_DIST += README \
	ChangeLog \
	TODO

# just re-use parent srcdir's template
local.cvsignore: $(srcdir)/../local.cvsignore
	$(LN_S) $< $@

include $(top_srcdir)/Make.global

