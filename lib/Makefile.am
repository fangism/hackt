# "lib/Makefile.am"
#	$Id: Makefile.am,v 1.25 2007/11/02 22:38:49 cto3 Exp $
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
	merge.hac \
	mux.hac

vim_DATA = \
	support/hac.vim \
	hac.vimrc

CLEANFILES += hac.vimrc

EXTRA_DIST += README \
	ChangeLog \
	TODO \
	support/hac.vim \
	support/ChangeLog

hac.vimrc:
	{ $(ECHO) "\" source this from $$HOME/.vimrc to autoload hac syntax." ; \
	$(ECHO) "au BufRead,BufNewFile *.hac set filetype=hac" ; \
	$(ECHO) "au BufRead,BufNewFile *.act set filetype=hac" ; \
	$(ECHO) "au BufRead,BufNewFile *.actmx set filetype=hac" ; \
	$(ECHO) "au! Syntax hac source $(vimdir)/hac.vim" ;} > $@

#################### CHP function library section #############################

# flags copied from $(top_srcdir)/src/Makefile.am
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

# BUG-libtool-ltdl?
# Only on Darwin, stdchpfn.la fails to lt_dlopen from test directories, 
# probably something to do with the way shared libraries are linked.
# Darwin .la archives don't have/need relink_commands like linux/freebsd libs.
# Bottom-line: for testing on Darwin, we must use a convenience library module, 
# named stdchpfnc.la here, to distinguish from the installed library.  
# NOTE: this issue is 'fixed' by passing addition library search paths
# (pointing to this directory) to the right environment variables.
# See test/Make.test-chpsim for CHPSIM_TEST_ENV for more notes.

pkglib_LTLIBRARIES = libstdchpfn.la stdchpfn.la

EXTRA_DIST += libchpfn/TODO

# this is just a normal non-module library
# MAINTAINENCE: don't forget to list new source files in 
#	dox/chpsim/Makefile.am for automatic documentation (texinfo)
libstdchpfn_la_SOURCES = \
	libchpfn/assert.cc \
	libchpfn/assert.h \
	libchpfn/bitmanip.cc \
	libchpfn/bitmanip.h \
	libchpfn/conditional.cc \
	libchpfn/conditional.h \
	libchpfn/io.cc \
	libchpfn/io.h \
	libchpfn/OS.cc \
	libchpfn/OS.h \
	libchpfn/string.cc \
	libchpfn/string.h
# libstdchpfn_la_LIBADD = ../src/libhackt.la

nobase_pkginclude_HEADERS = \
	libchpfn/assert.h \
	libchpfn/bitmanip.h \
	libchpfn/conditional.h \
	libchpfn/io.h \
	libchpfn/OS.h \
	libchpfn/string.h

# install a copy of the source for reference (is libdir appropriate?)
pkglib_DATA = libchpfn/stdchpfn.cc

# this is the module to be dlopened by chpsim
stdchpfn_la_SOURCES = libchpfn/stdchpfn.cc
stdchpfn_la_LIBADD = libstdchpfn.la
stdchpfn_la_LDFLAGS = -module
# this LIBADD dependency is what causes dlopen to fail on Darwin, 
# because the .libs objdir needs to be explicitly added to the library path.

###############################################################################

local.cvsignore:
	$(TOUCH) $@

DISTCLEANFILES += local.cvsignore
# will automatically pickup in AUTOIGNORE

include $(top_srcdir)/Make.global

