# "lib/Makefile.am"
#	$Id: Makefile.am,v 1.20 2007/08/15 02:48:33 fang Exp $
# Makefile for standard hackt library files.  
# All files managed herein should be architecture-independent, 
# and appropriate for installation in $(datadir).

include $(top_srcdir)/Make.stddef

AUTOMAKE_OPTIONS += subdir-objects
SUBDIRS = mk

# TODO: split off subdirectories into versions, like gcc?
# automake provides pkgdatadir = $(datadir)/hackt

hacktlibdir = $(pkgdatadir)/lib
# this is NOT the architecture-dependent pkglibdir
# nothing should be architecture-dependent, not even precompiled headers

vimdir = $(pkgdatadir)/vim

# library writers: add stuff here that you want to be installed standard
# TODO: organize a standard structure for library includes
dist_hacktlib_DATA = \
	renv.hac \
	logic.hac \
	channel.hac \
	env.hac \
	buffer.hac \
	buffer_init.hac \
	copy.hac \
	sync.hac \
	alternator.hac \
	split.hac \
	merge.hac

vim_DATA = \
	support/hac.vim \
	hac.vimrc

CLEANFILES += hac.vimrc

EXTRA_DIST += README \
	ChangeLog \
	TODO \
	support/hac.vim

hac.vimrc:
	{ $(ECHO) "\" source this from $$HOME/.vimrc to autoload hac syntax." ; \
	$(ECHO) "au BufRead,BufNewFile *.hac set filetype=hac" ; \
	$(ECHO) "au! Syntax hac source $(vimdir)/hac.vim" ;} > $@

#################### CHP function library section #############################

# extremely anal-retentive
if HAVE_GXX
# our good friend, g++
GENERAL_WARN_FLAGS = @FANG_WARN_FLAGS@
WARN_CFLAGS = @FANG_WARN_CFLAGS@
WARN_CXXFLAGS = @FANG_WARN_CXXFLAGS@
# -Wsign-promo
DIALECT_FLAGS = @FANG_DIALECT_FLAGS@
PIPE_FLAG = -pipe
else
# non-GNU compiler: your guess is as good as mine
# pass in whatever happened to succeed from configure tests
GENERAL_WARN_FLAGS = @FANG_WARN_FLAGS@
WARN_CFLAGS = @FANG_WARN_CFLAGS@
WARN_CXXFLAGS = @FANG_WARN_CXXFLAGS@
DIALECT_FLAGS = @FANG_DIALECT_FLAGS@
PIPE_FLAG =
endif

AM_CPPFLAGS = -I $(top_srcdir)/src
AM_CXXFLAGS = $(PIPE_FLAG) $(DIALECT_FLAGS) $(WARN_CXXFLAGS) $(GENERAL_WARN_FLAGS)

pkglib_LTLIBRARIES = libstdchpfn.la stdchpfn.la
EXTRA_DIST += libchpfn/TODO

# this is just a normal non-module library
# MAINTAINENCE: don't forget to list new source files in 
#	dox/chpsim/Makefile.am for automatic documentation (texinfo)
libstdchpfn_la_SOURCES = \
	libchpfn/assert.cc \
	libchpfn/assert.h \
	libchpfn/conditional.cc \
	libchpfn/conditional.h

nobase_pkginclude_HEADERS = \
	libchpfn/assert.h \
	libchpfn/conditional.h

# install a copy of the source for reference (is libdir appropriate?)
pkglib_DATA = libchpfn/stdchpfn.cc

# this is the module to be dlopened by chpsim
stdchpfn_la_SOURCES = libchpfn/stdchpfn.cc
stdchpfn_la_LIBADD = libstdchpfn.la
stdchpfn_la_LDFLAGS = -module

###############################################################################

local.cvsignore:
	$(TOUCH) $@

DISTCLEANFILES += local.cvsignore
# will automatically pickup in AUTOIGNORE

include $(top_srcdir)/Make.global

