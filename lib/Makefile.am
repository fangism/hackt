# "lib/Makefile.am"
#	$Id: Makefile.am,v 1.42 2011/05/07 03:43:40 fang Exp $
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

# TODO: provide a path override to KDE home
# does not really belong here
katesyndir = $(datadir)/katepart/syntax

# library writers: add stuff here that you want to be installed standard
# TODO: organize a standard structure for library includes
dist_hacktlib_DATA = \
	renv.hac \
	logic.hac \
	connect.hac \
	channel.hac \
	channel_ledr.hac \
	channel_bd.hac \
	env.hac \
	buffer.hac \
	buffer_init.hac \
	copy.hac \
	sync.hac \
	alternator.hac \
	split.hac \
	merge.hac \
	mux.hac

# vim syntax highlight support
vim_DATA = \
	support/hac.vim \
	hac.vimrc

# KDE editor syntax highlight support
dist_katesyn_DATA = \
	support/hac.xml

CLEANFILES += hac.vimrc

dist_bin_SCRIPTS = \
	scripts/deps_to_dot.awk \
	scripts/hacspice-pp.sed \
	scripts/prsim_timed_backtrace.awk \
	scripts/prsim_timed_fwdtrace.awk \
	scripts/prsimrc_alter_instance.awk \
	scripts/prsimlog_to_evcd.awk \
	scripts/prsimlog_to_evcd_ports.awk \
	scripts/prsimlog_to_evcd.sh \
	scripts/spice_cell_count.awk \
	scripts/spice_instance_tree.awk \
	scripts/spice_line_join.awk \
	scripts/spice-to-hac.awk \
	scripts/spice_to_dot.awk \
	scripts/spice_to_pdf.sh \
	scripts/spice_to_verilog_quick.awk \
	scripts/spice_tsort.sh \
	scripts/spice_type_deps.awk \
	scripts/spice_type_deps_to_pdf.sh \
	scripts/spice_unique_leaves.awk \
	scripts/spice-wrap.awk \
	verilog/vpp.sh \
	verilog/vcs_scandepend.sh \
	verilog/wrap_verilog_modules_to_hacprsim.awk

# distribute these, but do not install
dist_noinst_SCRIPTS = \
	compat/hac_fix_vt.sh \
	compat/upgrade_hacprsim_channel_commands.awk

EXTRA_DIST += README \
	ChangeLog \
	TODO \
	compat/README \
	support/hac.vim \
	support/ChangeLog

HAC_VIM_SUFFIXES = .hac .hacf .act .actf .actmx .prs .prsf .chp .chpf

hac.vimrc: Makefile
	{ $(ECHO) "\" source this from $$HOME/.vimrc to autoload hac syntax." ; \
	$(ECHO) "\" .act* is for a close dialect, ACT" ; \
	$(ECHO) "\" .*f is for sources preprocessed (flattened) with hacpp" ; \
	for e in $(HAC_VIM_SUFFIXES) ; \
	do $(ECHO) "au BufRead,BufNewFile *.$$e set filetype=hac" ; \
	done ; \
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
	libchpfn/assert.hh \
	libchpfn/bitmanip.cc \
	libchpfn/bitmanip.hh \
	libchpfn/conditional.cc \
	libchpfn/conditional.hh \
	libchpfn/io.cc \
	libchpfn/io.hh \
	libchpfn/OS.cc \
	libchpfn/OS.hh \
	libchpfn/numeric.cc \
	libchpfn/numeric.hh \
	libchpfn/string.cc \
	libchpfn/string.hh
# libstdchpfn_la_LIBADD = ../src/libhackt.la

nobase_pkginclude_HEADERS = \
	libchpfn/assert.hh \
	libchpfn/bitmanip.hh \
	libchpfn/conditional.hh \
	libchpfn/io.hh \
	libchpfn/OS.hh \
	libchpfn/string.hh

# install a copy of the source for reference (is libdir appropriate?)
nobase_pkgdata_DATA = libchpfn/stdchpfn.cc

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

