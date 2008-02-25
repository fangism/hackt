# "configure.ac"
# Process this file with "autoconf" to generate the "configure" script.
# (Prior to 2005/06/19, this file was "configure.in", 
#	in case one should want to trace back the CVS revision history.)
#
#	$Id: configure.ac,v 1.169.2.3 2008/02/25 08:23:37 fang Exp $

# Hint: Try running "autoscan" for suggestions on what should go in here.  
# Results of autoscan appear in "configure.scan".  

AC_PREREQ(2.59)
dnl also works with autoconf 2.60 now
AC_INIT(hackt, 0.1.4-devel-20071126, fangism@users.sourceforge.net)
dnl prefer to push this into src directory, 
dnl and have "src/config.h" wrap around this.  
AC_CONFIG_HEADERS([src/__config__.h])
AC_CONFIG_AUX_DIR(config)
dnl AM_CONFIG_HEADER(config.h)	dnl obsolete macro
AC_CONFIG_SRCDIR([src/main/main_funcs.cc])

# don't need this yet
dnl AC_CONFIG_SUBDIRS()

# Propagate the make warning flags recursively
AM_INIT_AUTOMAKE([1.9 -Wall -Werror])

# Enable or disable automatic reconfiguring.  
# AM_MAINTAINER_MODE

# Disabling shared library linking may speed up beta-testing and building.  
# AC_DISABLE_SHARED

AC_CANONICAL_HOST
if test "$AWK" = "mawk" ; then
  AC_MSG_WARN([mawk is known to die on valid input, please try any other awk!])
fi
HACKT_AUTO_CVSIGNORE

AC_CHECK_PROG([SH], sh, sh)
AC_PATH_PROG(SH_PATH, sh)
dnl AM_TRUE is always true, needed to fake conditional Makefile
AM_CONDITIONAL(ALWAYS_TRUE, true)

# CALLMAKE is intended for shell scripts' invocations of make
# NOT intended for direct use in configured Makefiles, because
# MAKE is more appropriate and configured for automake already.
AC_CHECK_PROGS([CALLMAKE], gmake make)

# basic program checks
AC_PROG_LN_S
AC_CHECK_PROG([CAT], cat, cat)
AC_CHECK_PROG([CPP_TRADITIONAL], cpp, cpp)
AC_CHECK_PROG([CHMOD], chmod, chmod)
# CVS is not required, but will enable some handy targets
AC_CHECK_PROG([CVS], cvs, cvs)
AM_CONDITIONAL(HAVE_CVS, test -n "$CVS")
AC_CHECK_PROG([DATE], date, date)
AC_CHECK_PROG([DIFF], diff, diff)
AC_CHECK_PROG([PATCH], patch, patch)
AC_CHECK_PROG([FIND], find, find)
AC_CHECK_PROG([GREP], grep, grep)
AC_CHECK_PROG([HEXDUMP], hexdump, hexdump)
AC_CHECK_PROG([ECHO], echo, echo)
AC_CHECK_PROG([SORT], sort, sort)
AC_CHECK_PROG([CUT], cut, cut)
AC_CHECK_PROG([PASTE], paste, paste)
AC_CHECK_PROG([HEAD], head, head)
AC_CHECK_PROG([TAIL], tail, tail)
AC_CHECK_PROG([TEE], tee, tee)
# AC_CHECK_PROG([PWD], pwd, pwd)
#	checking for PWD doesn't work as expected...
AC_CHECK_PROG([SED], sed, sed)
AC_CHECK_PROG([TOUCH], touch, touch)
FANG_PROG_SLEEP
AC_CHECK_PROG([INDENT], indent, indent)
AC_CHECK_PROGS([M4], gm4 m4, m4)
AC_CHECK_PROGS([XARGS], xargs, xargs)

# AC_PROG_PERL
AC_CHECK_PROG([PERL], perl, perl)
AC_PATH_PROG([PERL_PATH], perl)

# documentation support, macros efined in "config/documentation.m4"
DOC_CHECK_PROG_DOXYGEN
DOC_CHECK_PROGS_GRAPHVIZ
FANG_CHECK_PROGS_TETEX
dnl already checked and defined by texinfos.am
dnl DOC_CHECK_PROG_DVIPS
DOC_CHECK_PROG_PS2PDF
DOC_CHECK_PROG_PS2EPSI
AC_CHECK_PROG([PSNUP], psnup, psnup)
AC_CHECK_PROG([INFO], info, info)
AM_CONDITIONAL(HAVE_INFO, test -n "$INFO" )
AC_CHECK_PROG([MAKEINFO], makeinfo, makeinfo)
dnl covered by texinfos.am already
dnl AC_CHECK_PROG([TEXI2DVI], texi2dvi, texi2dvi)
dnl AC_CHECK_PROG([TEXI2PDF], texi2pdf, texi2pdf)
AC_CHECK_PROG([TEXI2HTML], texi2html, texi2html)

# figure related stuff
FANG_CHECK_PROGS_XFIG
dnl checks for xfig, transfig, fig2dev

AC_CHECK_FILES([/dev/null])
dnl standard terminal devices for redirection (test doesn't work)
dnl AC_CHECK_FILES([/dev/stdin /dev/stdout /dev/stderr])

# checking for old CAST tools for regression and backwards compatibility tests
# no big deal if they don't exist...
# NOTE: AM_CONDITIONAL is usable in generation of AC_CONFIG_FILES
#	just use @VAR_TRUE@ instead of the if ... version in automake
AC_CHECK_PROG([PRSIM], prsim, prsim)
AC_CHECK_PROG([LVS], lvs, lvs)
AC_CHECK_PROG([CFLAT], cflat, cflat)
AM_CONDITIONAL(HAVE_PRSIM, test -n "$PRSIM" )
AM_CONDITIONAL(HAVE_LVS, test -n "$LVS" )
AM_CONDITIONAL(HAVE_CFLAT, test -n "$CFLAT" )

# Lex/Yacc support
# note: autoconf-2.60 overrides this with its own built-in YACC check
# but it doesn't break anything
HACKT_ARG_VAR_LEX
HACKT_ARG_VAR_YACC
HACKT_AM_CONDITIONAL_HAVE_YACC
HACKT_YACC_PURE_PARSER
HACKT_YACC_VERSION
HACKT_LEX_VERSION

AC_CHECK_PROG([PKG_CONFIG], pkg-config, pkg-config)
AM_CONDITIONAL(HAVE_PKG_CONFIG, test -n "$PKG_CONFIG" )
# interface generator, no longer needed
dnl AC_CHECK_PROG([SWIG], swig, swig)
dnl AM_CONDITIONAL(HAVE_SWIG, test -n "$SWIG" )

# comment out until time for release
# this causes too many rebuilds during development
# AC_DEFINE_UNQUOTED(HACKT_BUILD_DATE, "`$DATE`", [Define the build date.])

# This takes care of many standard C[++] compiler configuration checks.  
AC_PROG_RANLIB
dnl libtool dlopen features
AC_LIBLTDL_CONVENIENCE
AC_SUBST(LTDLINCL)
AC_SUBST(LIBLTDL)
AC_LIB_LTDL
AC_LIBTOOL_DLOPEN
dnl to support dlopen plug-ins and module shared objects/libraries
dnl AC_DISABLE_STATIC
AC_PROG_LIBTOOL
AC_CONFIG_SUBDIRS(libltdl)
dnl now, always use verbose libtool script, instead of --silent
dnl HACKT_ARG_ENABLE_LIBTOOL_VERBOSE
# Automatically update if libtool script is outdated.  
# Adds corresponding dependencies to "Makefile.in".  
HACKT_ARG_VAR_LIBTOOL_FLAGS
AC_SUBST(LIBTOOL_DEPS)
dnl need these variables to augment test environment
LTDL_SHLIBPATH_VAR=$libltdl_cv_shlibpath_var
AC_SUBST(LTDL_SHLIBPATH_VAR)
LTDL_OBJDIR=$libltdl_cv_objdir
AC_SUBST(LTDL_OBJDIR)


if test "$enable_shared" = "no" ; then
  AC_MSG_WARN([You requested disabling shared libraries, however some features of this package require shared libraries, so certain tests will fail!])
fi

AC_ARG_VAR(ACLOCAL_FLAGS, [Additional aclocal flags, for bootstrap only.])

dnl defined in config.hackt.m4
HACKT_ARG_ENABLE_OBJECT_ALIGN_CHECKING
HACKT_ARG_ENABLE_GOD_MODE
dnl defined in config/header_validation.m4
FANG_ARG_ENABLE_HEADER_VALIDATION
dnl test control
HACKT_ARG_ENABLE_CHECK_TESTS
HACKT_ARG_ENABLE_RANDOM_TESTS
HACKT_ARG_ENABLE_INSTALL_TESTS
dnl from "config/rl_el.m4"
FANG_ARG_WITH_READLINE_EDITLINE
HACKT_ARG_WITH_GMP
HACKT_ARG_WITH_XDR
HACKT_ARG_WITH_VPI

dnl whether or not to build universal binary (config/hackt.m4)
dnl not actually implemented yet
HACKT_ARG_ENABLE_MULTILIB

dnl AC_ARG_ENABLE for building documentation (on by default)
dnl from "config/documentation.m4"
DOC_ARG_ENABLE_DOCS

HACKT_ARG_ENABLE_VERBOSE_CHECK

dnl enable or disable fun modules ("config/hackt.m4")
HACKT_ARG_ENABLE_FUN

dnl define WORDS_BIGENDIAN if is big endian
dnl not actually needed in this project yet
AC_C_BIGENDIAN

AC_PROG_CXX

dnl from "config/cxx.m4"
FANG_CXX_VERSION

dnl checkpoint
AC_CACHE_SAVE

dnl we need these anal flags for checking for certain charateristics
dnl TODO: what are the equivalent flags for other compilers?
dnl from "config/cxx.m4"
FANG_ANAL_COMPILE_FLAGS
FANG_AM_FLAGS

dnl which set of pre-determined compiler flags to apply?
FANG_CXX_COMPILER
dnl TODO: add other conditionals for other common compilers
dnl XLC++, ICC, HPCC, etc...

AM_PROG_CC_C_O
AC_ISC_POSIX

dnl checks tons of standard C++ headers in various locations
dnl from "config/cxx_STL.m4"
dnl from this point to the end, we also apply ANAL_FLAGS before CFLAGS
dnl and CXXFLAGS to reflect what will be used during the build process.  
FANG_HEADER_STDCXX
dnl AC_MSG_NOTIFY([Checking library functions and types with $ANAL_FLAGS])

FANG_CXX_ATTRIBUTE_UNUSED
FANG_CXX_ATTRIBUTE_UNUSED_CTOR
FANG_CXX_ATTRIBUTE_CONST
FANG_CXX_ATTRIBUTE_PURE
FANG_CXX_ATTRIBUTE_HOLY
FANG_CXX_ATTRIBUTE_NORETURN
FANG_CXX_ATTRIBUTE_MALLOC
FANG_CXX_ATTRIBUTE_PRECIOUS
FANG_CXX_ATTRIBUTE_VISIBILITY
FANG_CXX_ATTRIBUTE_PACKED
FANG_CXX_ATTRIBUTE_ALIGNED
FANG_CXX_ATTRIBUTE_ALIGNED_SIZE
FANG_CXX_ATTRIBUTE_EVIL
FANG_CXX_DEFAULT_TEMPLATE_TEMPLATE_PARAMETER_BINDING
FANG_CXX_TEMPLATE_FORMAL_BASE_CLASS

FANG_HASH_MAP_NAMESPACE
FANG_HASH_MAP_STYLE
FANG_CXX_STL_FUNCTIONAL_SELECT
FANG_CXX_STL_REVERSE_ITERATOR_COMPARISONS
FANG_CXX_STL_TREE
FANG_CXX_STD_IFSTREAM_DEV_STDIN
FANG_CXX_STD_OFSTREAM_DEV_STDOUT
FANG_CXX_STD_OFSTREAM_DEV_STDERR
FANG_CXX_STL_BITSET_EXTENSIONS
dnl check for compiler's built-in functions
FANG_C_BUILTIN_EXPECT

AC_LANG_PUSH(C++)
FANG_FUNC_BUILTIN_BITOPS
AC_LANG_POP(C++)
AC_CACHE_SAVE

# additional header checks
AC_HEADER_STDC
AC_CHECK_HEADERS([stddef.h stdlib.h stdint.h string.h unistd.h getopt.h])
AC_CHECK_HEADERS(pthread.h)
AC_CHECK_HEADERS([malloc.h])
AC_CHECK_HEADERS([ctype.h])
AC_CHECK_HEADERS([sys/stdtypes.h])
AC_HEADER_DIRENT
AC_CHECK_HEADERS([dirent.h sys/dirent.h])
AC_CHECK_HEADERS([signal.h sys/signal.h])
AC_CHECK_HEADERS([sys/stat.h])
AC_CHECK_HEADERS([fcntl.h sys/fcntl.h])

AC_CACHE_SAVE

dnl we deliberaly spare these basic check from Fang's anal compile flags
dnl suggestions from autoscan
AC_FUNC_ALLOCA			dnl wanted by the y.tab.c generated
AC_FUNC_CLOSEDIR_VOID		dnl wanted by src/util/dirent.cc
AC_FUNC_STAT			dnl wanted by src/util/file_status.h

# compiler characteristic checks
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_C_RESTRICT
AC_TYPE_SIZE_T
AC_TYPE_SIGNAL
AC_C_VOLATILE
FANG_TYPEDEF_FILE			dnl defined in "config/cc.m4"

# library functions
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([calloc free])
AC_MSG_CHECKING([for free food])
AC_MSG_RESULT([YES]);
AC_FUNC_SETVBUF_REVERSED

top_saved_CFLAGS="$CFLAGS"
top_saved_CXXFLAGS="$CXXFLAGS"

dnl ignore warnings for function checks
dnl CONFTEST flags are defined by FANG_CONFTEST_FLAGS from hackt.m4
CFLAGS="$CONFTEST_CFLAGS -Wno-error"
CXXFLAGS="$CONFTEST_CXXFLAGS -Wno-error"
dnl temporarily override ac_compile and ac_link to force checking in C++ mode
dnl language mode doesn't really matter because these test by linking
AC_LANG_PUSH(C++)
AC_CHECK_TYPES([ptrdiff_t])
AC_CHECK_TYPES([pid_t])
dnl AC_CHECK_FUNCS([sqrt])
FANG_FUNC_SQRT
dnl AC_CHECK_FUNCS([fmod fmodf])
FANG_FUNC_FMOD
FANG_FUNC_FMODF
AC_CHECK_FUNCS([getopt strsep strtok strtok_r strdup \]
dnl strsep is missing on Solaris 2.9
dnl <stdio.h>
	[fgets fputs fputc fgetc fopen fdopen fclose fflush ferror \]
	[feof clearerr open close fseek ftell rewind fgetpos fsetpos \]
	[tmpfile tmpnam tempnam mktemp mkstemp mkstemps mkdtemp \]
	[remove rename \]
	[setbuf setbuffer setlinebuf setvbuf \]
dnl <ctype.h>
	[isascii isspace isdigit isalpha isalnum islower isupper \]
	[iscntrl isgraph ishexnumber isideogram isnumber isphonogram \]
	[isspecial isprint ispunct isrune isxdigit tolower toupper \]
	[drand48 erand48 lrand48 nrand48 \]
	[mrand48 jrand48 srand48 seed48 lcong48 \]
	[system exec \]
	[atexit \]
	[getenv setenv putenv unsetenv \]
	[pipe fork vfork popen pclose dup dup2 \]
dnl vfork is not always available
	[wait waitpid wait3 wait4 \]
dnl wait3 and wait4 are not specificed by POSIX, avoid using them if possible
dnl the following are expected to be in glibc
	[fmemopen open_memstream]
)
FANG_CHECK_CXX_RAND48_FUNCS
FANG_CHECK_CXX_FDOPEN
FANG_CHECK_CXX_POPEN
AC_LANG_POP(C++)
dnl but keep errors for type checks.  
CFLAGS="$CONFTEST_CFLAGS"
CXXFLAGS="$CONFTEST_CXXFLAGS"

# sizeof checks, useful for checking ILP model
AC_LANG_PUSH(C++)
dnl testing in C++ mode coerces test to use CXXFLAGS
AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(uchar)
AC_CHECK_SIZEOF(u_char)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(ushort)
AC_CHECK_SIZEOF(u_short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(uint)
AC_CHECK_SIZEOF(u_int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(ulong)
AC_CHECK_SIZEOF(u_long)
dnl NOTE: o some systems, system headers protect long long, which causes
dnl the following test to *unexpectedly* pass even with -pedantic-errors
dnl This inconsistency between configure and use is quite annoying and 
dnl motivates us to automatically apply -Wno-long-long.
dnl Below, we call FANG_CXX_AUTO_NO_LONG_LONG after CXXFLAGS is restored.
AC_CHECK_SIZEOF(long long)
AC_CHECK_SIZEOF(ulonglong)
AC_CHECK_SIZEOF(float)
AC_CHECK_SIZEOF(double)
AC_CHECK_SIZEOF(long double)
AC_CHECK_SIZEOF(void*)
AC_CHECK_SIZEOF(size_t)
AC_CHECK_SIZEOF(int16)
AC_CHECK_SIZEOF(uint16)
AC_CHECK_SIZEOF(u_int16)
AC_CHECK_SIZEOF(int16_t)
AC_CHECK_SIZEOF(uint16_t)
AC_CHECK_SIZEOF(u_int16_t)
AC_CHECK_SIZEOF(int32)
AC_CHECK_SIZEOF(uint32)
AC_CHECK_SIZEOF(u_int32)
AC_CHECK_SIZEOF(int32_t)
AC_CHECK_SIZEOF(uint32_t)
AC_CHECK_SIZEOF(u_int32_t)
AC_CHECK_SIZEOF(int64)
AC_CHECK_SIZEOF(uint64)
AC_CHECK_SIZEOF(u_int64)
AC_CHECK_SIZEOF(int64_t)
AC_CHECK_SIZEOF(uint64_t)
AC_CHECK_SIZEOF(u_int64_t)
FANG_TYPE_QUAD_T_ARITHMETIC
FANG_TYPE_U_QUAD_T_ARITHMETIC
FANG_TYPE_EQUIV_SIZE_T_UNSIGNED_LONG
FANG_TYPE_EQUIV_UINT32_UNSIGNED_LONG
AC_LANG_POP(C++)

dnl test whether or not we're expecting 32b binari interface
dnl this is used to expect binary differences on non-LP32 platforms.
AM_CONDITIONAL(ABI_ILP32, test "$ac_cv_sizeof_size_t" = "4")

CFLAGS="$top_saved_CFLAGS"
CXXFLAGS="$top_saved_CXXFLAGS"

dnl detect dialect/test inconsistency and workaround
FANG_CXXFLAGS_AUTO_NO_LONG_LONG

AC_CACHE_SAVE

HACKT_CHECK_SANITY

dnl PACKAGE CHECKS
dnl readline or editline support
FANG_CHECK_READLINE
FANG_CHECK_EDITLINE
dnl check prototype of readline() if applicable
FANG_CHECK_READLINE_ARG_CONST

dnl check for X11 includes and libraries
hackt_have_x11=no
AC_PATH_X
if test -d "$ac_x_includes" ; then
  X11_INCLUDE="-I$ac_x_includes"
  if test -d "$ac_x_libraries" ; then
    hackt_have_x11=yes
  fi
fi
if test -d "$ac_x_libraries" ; then
  X11_LDPATH="-L$ac_x_libraries"
fi
AC_SUBST(X11_INCLUDE)
AC_SUBST(X11_LDPATH)
if test "$hackt_have_x11" = "yes" ; then
    AC_DEFINE(HAVE_X11, 1, [Define to 1 if we X11 includes/libs found])
fi
AM_CONDITIONAL(HAVE_X11, test "$hackt_have_x11" = "yes")


dnl check for guile-config
FANG_GUILE

dnl TODO: AC_SUBST these flags into separate options
dnl TODO: xdr
dnl for automake consumption
dnl order would matter if there are cross-conflicting headers installed
AUTO_CPPFLAGS="$EL_INCLUDE $RL_INCLUDE $NCURSES_INCLUDE $GMP_INCLUDE"
dnl order might matter in the presence of cross-conflicting libraries
AUTO_LDFLAGS="$EL_LDPATH $RL_LDPATH $NCURSES_LDPATH $GMP_LDPATH"
dnl order matters! we are taking responsibility for this
dnl rather than letting LIBS accumulate over checks.  
AUTO_LIBS="$EL_LIB $RL_LIB $NCURSES_LIB $GMP_LIB"
LIBS="$AUTO_LIBS"
dnl may decide not to use these, depending on how much control is needed
AC_SUBST(AUTO_CPPFLAGS)
AC_SUBST(AUTO_LDFLAGS)
dnl AC_SUBST(AUTO_LIBS)


dnl NOTE USED ANY MORE
dnl first, create a useful variable substitution script for use in config.status
dnl AC_CONFIG_FILES([config_subst.awk.sh:config/config_subst.awk],
dnl [dollar=$
dnl at=@
dnl cat > config_subst.awk.sh <<EOF
dnl #!$SHELL
dnl awk -f $srcdir/config/config_subst.awk $dollar$at
dnl EOF
dnl chmod +x config_subst.awk.sh
dnl ])

CONFIGDATE=`$DATE`
AC_SUBST(CONFIGDATE)

cat <<ACEOF
configuration summary:
	CONFIG_SHELL:		$CONFIG_SHELL
	CXX:			$CXX_VERSION
	CPPFLAGS:		$CPPFLAGS
	CXXFLAGS:		$CXXFLAGS
	LDFLAGS:		$LDFLAGS
	LEX:			$LEX_VERSION
	YACC:			$YACC_VERSION
	ltdl include:		$LTDLINCL
	ltdl libs:		$LIBLTDL
	readline include:	$RL_INCLUDE
	readline LDFLAGS:	$RL_LDPATH
	readline libs:		$RL_LIB
	editline include:	$EL_INCLUDE
	editline LDFLAGS:	$EL_LDPATH
	editline libs:		$EL_LIB
	ncurses include:	$NCURSES_INCLUDE
	ncurses LDFLAGS:	$NCURSES_LDPATH
	ncurses libs:		$NCURSES_LDPATH
	GMP include:		$GMP_INCLUDE
	GMP LDFLAGS:		$GMP_LDPATH
	GMP libs:		$GMP_LIB
	X11 include:		$X11_INCLUDE
	X11 LDFLAGS:		$X11_LDPATH
	guile config:	 	$GUILE_CONFIG_VERSION
	guile CPPFLAGS: 	$GUILE_CPPFLAGS
	guile LDFLAGS: 		$GUILE_LDFLAGS
ACEOF


# additional patch to Makefile to work around automake shortcoming
# recursive rm may fail if too many files in particular subdir.
# sed substitution forces POSIX-mode of tar to not be limited to 99 chars
# in full path names of files
AC_CONFIG_FILES([Makefile], 
	[patch -p0 < $srcdir/config/am__remove_distdir.diff
	sed '/^am__tar/s/chof/chf/' Makefile > Makefile.tmp
	mv Makefile.tmp Makefile
	])
dnl ignore errors during attempt to make directories in parallel
AC_CONFIG_FILES([src/Makefile],
	[sed '/test.*mkdir.*DESTDIR/s/test/-test/g' src/Makefile > src/Makefile.tmp && mv src/Makefile.tmp src/Makefile])
AC_CONFIG_FILES([
	scripts/Makefile
	src/util/test/Makefile
	src/scm/Makefile
	test/Makefile
	test/lexer/Makefile
	test/parser/Makefile
	test/parser/basic/Makefile
	test/parser/namespace/Makefile
	test/parser/param/Makefile
	test/parser/array/Makefile
	test/parser/process/Makefile
	test/parser/prs/Makefile
	test/parser/chp/Makefile
	test/parser/datatype/Makefile
	test/parser/channel/Makefile
	test/parser/template/Makefile
	test/parser/typedef/Makefile
	test/parser/connect/Makefile
	test/parser/flow/Makefile
	test/prsim/Makefile
	test/prsim/vpi/Makefile
	test/chpsim/Makefile
	test/chpsim/testlib/Makefile
	test/chpsim/router/Makefile
	test/chpsim/installtest/Makefile
	test/chpsim/libinstalltest/Makefile
	test/guile/Makefile
	test/lib/Makefile
	test/benchmarks/Makefile
	test/quarantine/Makefile
	dox/Makefile
	dox/mk/Makefile
	dox/lang/Makefile
	dox/util/Makefile
	dox/objfmt/Makefile
	dox/man/Makefile
	dox/tutorial/Makefile
	dox/cast2hac/Makefile
	dox/prsim/Makefile
	dox/chpsim/Makefile
	lib/Makefile
	lib/mk/Makefile
	hackt.doxygen.config
	installcheck.mk
])
AC_CONFIG_FILES([hackt-config],
	[chmod +x hackt-config])
AC_CONFIG_FILES([scripts/replace_if_changed.sh],
	[chmod +x scripts/replace_if_changed.sh])
AC_CONFIG_FILES([src/util/test/hash_map_iterator_O2_bug_test.test],
	[chmod +x src/util/test/hash_map_iterator_O2_bug_test.test])
AC_CONFIG_FILES([src/scripts/deps_to_dot.awk],
	[chmod +x src/scripts/deps_to_dot.awk])
AC_CONFIG_FILES([install_transform.sh],
	[chmod +x install_transform.sh])
AC_CONFIG_FILES([test/POM-header-filter.awk],
	[chmod +x test/POM-header-filter.awk])
AC_CONFIG_FILES([test/test-series.sh],
	[chmod +x test/test-series.sh])

dnl the AC_CONFIG_HEADERS macro does not perform AC_SUBST-itution
dnl so we have to fall back onto AC_CONFIG_FILES :(
dnl It wouldve been nice to have difference checking from AC_CONFIG_HEADERS...
dnl work on this nicety later...

dnl no input file needed, just referencing arbitrary existing file
dnl AC_CONFIG_FILES([configdate.h:config/macro-value.h.in],
dnl	[out=configdate.h
dnl	date=`./config_subst.awk.sh -v var=DATE config.status`
dnl	sed 's/MACRO/CONFIGDATE/g' $srcdir/config/macro-value.h.in | \
dnl		sed "s/VALUE/\"`$date`\"/g" > $out
dnl	])

AC_CONFIG_FILES([configdate.h src/buildhost.h])
AC_CONFIG_FILES([src/cxxflags.h src/cxx_version.h])
AC_CONFIG_FILES([src/lexer/lex_version.h src/parser/yacc_version.h])
AC_CONFIG_FILES([src/parser/hackt-parse.yy])
AC_CONFIG_FILES([src/parser/instref-parse.yy])
AC_CONFIG_FILES([src/parser/type-parse.yy])
AC_CONFIG_FILES([src/guile/config_version.h])
AC_CONFIG_FILES([src/util/lang/CX/cx-parse.yy])
AC_CONFIG_FILES([src/scripts/init_hackt.sh],
	[chmod +x src/scripts/init_hackt.sh])
AC_CONFIG_FILES([src/scripts/init_hackt.csh],
	[chmod +x src/scripts/init_hackt.csh])

dnl initialize and reset header check dependencies
dnl also depend on Makefile?
dnl can't run make until depfiles has run! doh... work on this later...
dnl
dnl AC_CONFIG_FILES([src/headers_deps.make],
dnl 	[out=src/headers_deps.make
dnl 	(cd src && make echo-header-depbases) | \
dnl 		awk -f $srcdir/src/scripts/automake_include.awk >> $out
dnl 	])

dnl auxiliary directories
AC_CONFIG_COMMANDS([mkdir-builddir],
[builddirlist='
dox/chpsim/figs
src/util/STL/.deps
src/util/lang/lexyacc
'
for d in $builddirlist
do
	test -d $d || mkdir -p $d
done
])

AC_OUTPUT

