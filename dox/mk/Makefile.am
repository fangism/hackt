# "dox/mk/Makefile.am"
#	$Id: Makefile.am,v 1.1.20.2 2005/07/23 04:27:54 fang Exp $

include $(top_srcdir)/Make.stddef

# no targets are really built in this directory
# this directory just includes files common to other subdirectories of dox.

EXTRA_DIST += maketexdepend.awk maketexdepend.tex Make.latex

AUTO_IGNORE += local.cvsignore
DISTCLEANFILES += local.cvsignore

local.cvsignore: $(srcdir)/../local.cvsignore
	$(CP) $< $@

include $(top_srcdir)/Make.global

