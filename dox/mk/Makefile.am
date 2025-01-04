# "dox/mk/Makefile.am"
#	$Id: Makefile.am,v 1.9 2007/08/09 03:47:07 fang Exp $

include $(top_srcdir)/Make.stddef

# no targets are really built in this directory
# this directory just includes files common to other subdirectories of dox.

EXTRA_DIST += Make.latex scantexdepend.awk \
	updatefig.awk updatefig.mk

AUTO_IGNORE += local.gitignore
DISTCLEANFILES += local.gitignore

local.gitignore: $(srcdir)/../local.gitignore
	$(CP) $< $@

include $(top_srcdir)/Make.global

