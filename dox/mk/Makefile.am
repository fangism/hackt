# "dox/mk/Makefile.am"
#	$Id: Makefile.am,v 1.8 2007/06/12 05:12:30 fang Exp $

include $(top_srcdir)/Make.stddef

# no targets are really built in this directory
# this directory just includes files common to other subdirectories of dox.

EXTRA_DIST += Make.latex scantexdepend.awk \
	updatefig.awk updatefig.mk

AUTO_IGNORE += local.cvsignore
DISTCLEANFILES += local.cvsignore

local.cvsignore: $(srcdir)/../local.cvsignore
	$(CP) $? $@

include $(top_srcdir)/Make.global

