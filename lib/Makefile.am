# "lib/Makefile.am"
#	$Id: Makefile.am,v 1.16 2007/06/08 05:30:36 fang Exp $
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

vimdir = $(pkgdatadir)/vim

# library writers: add stuff here that you want to be installed standard
# TODO: organize a standard structure for library includes
dist_hacktlib_DATA = \
	renv.hac \
	logic.hac \
	channel.hac \
	env.hac \
	buffer.hac \
	copy.hac \
	sync.hac \
	alternator.hac \
	split.hac \
	merge.hac

# Makefile and automake templates
dist_make_DATA = \
	mk/hackt.mk \
	mk/hackt-suffixes.am \
	mk/hackt-rules.am

dist_vim_DATA = support/hac.vim
vim_DATA = hac.vimrc

CLEANFILES += hac.vimrc

EXTRA_DIST += README \
	ChangeLog \
	TODO

hac.vimrc:
	{ $(ECHO) "\" source this from $$HOME/.vimrc to autoload hac syntax." ; \
	$(ECHO) "au BufRead,BufNewFile *.hac set filetype=hac" ; \
	$(ECHO) "au! Syntax hac source $(vimdir)/hac.vim" ;} > $@

local.cvsignore:
	$(TOUCH) $@

DISTCLEANFILES += local.cvsignore
# will automatically pickup in AUTOIGNORE

include $(top_srcdir)/Make.global

