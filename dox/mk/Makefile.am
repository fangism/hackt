# "dox/mk/Makefile.am"
#	$Id: Makefile.am,v 1.7.24.1 2007/06/06 05:02:20 fang Exp $

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

