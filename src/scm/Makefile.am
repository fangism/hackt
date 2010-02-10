# "src/scm/Makefile.am"
#	$Id: Makefile.am,v 1.3.16.1 2010/02/10 06:43:11 fang Exp $

include $(top_srcdir)/Make.stddef
include $(top_srcdir)/installdirs.mk

# install as $(pkgdatadir)/scm/hackt/...
nobase_dist_scm_DATA = \
	hacguile.scm \
	hacchpsimguile.scm \
	hackt/algorithm.scm \
	hackt/streams.scm \
	hackt/rb-tree.scm \
	hackt/hackt-primitives.scm \
	hackt/hackt.scm \
	hackt/chpsim-primitives.scm \
	hackt/chpsim.scm \
	hackt/chpsim-trace-primitives.scm \
	hackt/chpsim-trace.scm

EXTRA_DIST += README \
	ChangeLog \
	TODO

# just re-use parent srcdir's template
local.cvsignore: $(srcdir)/../local.cvsignore
	$(LN_SF) $< $@

include $(top_srcdir)/Make.global

