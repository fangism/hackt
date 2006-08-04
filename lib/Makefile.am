# "lib/Makefile.am"
#	$Id: Makefile.am,v 1.9 2006/08/04 20:20:03 fang Exp $
# Makefile for standard hackt library files.  
# All files managed herein should be architecture-independent, 
# and appropriate for installation in $(datadir).

include $(top_srcdir)/Make.stddef

# TODO: split off subdirectories into versions, like gcc?
# automake provides pkgdatadir = $(datadir)/hackt

hacktlibdir = $(pkgdatadir)/lib
# this is NOT the architecture-dependent pkglibdir
# nothing should be architecture-dependent, not even precompiled headers
makedir = $(pkgdatadir)/mk

# library writers: add stuff here that you want to be installed standard
# TODO: organize a standard structure for library includes
dist_hacktlib_DATA = \
	renv.hac \
	logic.hac \
	channel.hac \
	env.hac \
	buffer.hac

# Makefile and automake templates
dist_make_DATA = \
	mk/hackt.mk \
	mk/hackt-suffixes.am \
	mk/hackt-rules.am

EXTRA_DIST += README \
	ChangeLog \
	TODO

local.cvsignore:
	$(TOUCH) $@

DISTCLEANFILES += local.cvsignore
# will automatically pickup in AUTOIGNORE

include $(top_srcdir)/Make.global

