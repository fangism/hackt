# "configure.ac"
# Process this file with "autoconf" to generate the "configure" script.
# (Prior to 2005/06/19, this file was "configure.in", 
#	in case one should want to trace back the CVS revision history.)
#
#	$Id: configure.ac,v 1.15.2.2 2005/11/11 02:43:23 fang Exp $

# Hint: Try running "autoscan" for suggestions on what should go in here.  
# Results of autoscan appear in "configure.scan".  

AC_PREREQ(2.59)
# using art++ results in art-- :(
AC_INIT(hackt, 0.1.4-devel-20051110, fangism@users.sourceforge.net)
AC_CONFIG_AUX_DIR(config)
AM_CONFIG_HEADER(config.h)
AC_CONFIG_SRCDIR([src/main/main_funcs.cc])

# ripped from ngspice's configure.in
# TODO: rip the library and header checks too, to be invoked later
# It would be a good idea to see how they handled compatiblity between the two
dnl --with-readline:  Includes GNU readline support into CLI.  Default is "no".
dnl Including readline into ngspice is a violation of GPL license. It's use
dnl is discouraged.
AC_ARG_WITH(readline,
	AS_HELP_STRING([--with-readline[=yes/no]],
		[Enable GNU readline support for CLI.  Default=no.]))

dnl --with-editline:  Includes BSD Editline support into CLI.  Default is "no".
AC_ARG_WITH(editline,
	AS_HELP_STRING([--with-editline[=yes/no]],
		[Enable BSD editline support for CLI.  Default=no.]))

dnl readline and editline cannot both be enabled
if test "$with_editline" = "yes"; then
	if test "$with_readline" = "yes"; then
		AC_MSG_ERROR(Readline and editline cannot both be enabled)
	fi
fi

AC_CANONICAL_HOST
AC_CHECK_PROG([SH], sh, sh)
AC_PATH_PROG(SH_PATH, sh)

# CALLMAKE is intended for shell scripts' invocations of make
# NOT intended for direct use in configured Makefiles, because
# MAKE is more appropriate and configured for automake already.
AC_CHECK_PROGS([CALLMAKE], gmake make)

# basic program checks
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
AC_CHECK_PROG([ECHO], echo, echo)
AC_CHECK_PROG([SORT], sort, sort)
AC_CHECK_PROG([CUT], cut, cut)
AC_CHECK_PROG([PASTE], paste, paste)
AC_CHECK_PROG([HEAD], head, head)
AC_CHECK_PROG([TAIL], tail, tail)
# AC_CHECK_PROG([PWD], pwd, pwd)
#	checking for PWD doesn't work as expected...
AC_CHECK_PROG([SED], sed, sed)
AC_CHECK_PROG([TOUCH], touch, touch)

# comment out until time for release
# this causes too many rebuilds during development
# AC_DEFINE_UNQUOTED(HACKT_BUILD_DATE, "`$DATE`", [Define the build date.])

# don't need this yet
# AC_CONFIG_SUBDIRS

# Propagate the make warning flags recursively
AM_INIT_AUTOMAKE([1.9 -Wall -Werror])

# Enable or disable automatic reconfiguring.  
# AM_MAINTAINER_MODE

# Disabling shared library linking may speed up beta-testing and building.  
# AC_DISABLE_SHARED

# This takes care of many standard C[++] compiler configuration checks.  
AC_PROG_RANLIB
AC_PROG_LIBTOOL

# Automatically update if libtool script is outdated.  
# Adds corresponding dependencies to "Makefile.in".  
AC_SUBST(LIBTOOL_DEPS)

# appends --silent option to all libtool invocations
LIBTOOL="$LIBTOOL --silent"

AC_PROG_CXX
# need to set LANG to C++ to check for C++ headers
# TODO: should use AC_SAVE_LANG and AC_RESTORE_LANG
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AM_PROG_CC_C_O
AC_ISC_POSIX

# additional header checks
AC_CHECK_HEADERS([stddef.h stdlib.h string.h unistd.h getopt.h])
AC_CHECK_HEADERS(pthread.h)
AC_CHECK_HEADERS([dirent.h sys/dirent.h])
# C++ wrappers to standard C headers
AC_CHECK_HEADERS([cstddef cstdlib cstdio cstring cassert cmath])
# checking all possible locations of (some equivalent) header files
# spanning versions gcc-2.95 to 4.x
# I bet you didn't know some of these locations were ever used!
# However, you're not supposed to use anything older than gcc-3.3 anyways...

# where is that damn hash_map?
AC_CHECK_HEADERS([ext/hash_map hash_map])
# used mainly in "src/util/hash_specializations.h"
AC_CHECK_HEADERS([ext/stl_hash_fun.h ext/hash_fun.h stl_hash_fun.h])
AC_CHECK_HEADERS([ext/hash_set hash_set])
AC_CHECK_HEADERS([ext/hashtable.h tr1/hashtable hashtable.h])
AC_CHECK_HEADERS([tr1/unordered_map tr1/unordered_set])
AC_CHECK_HEADERS([ext/functional tr1/functional])
# used in "src/util/string_fwd.h"
AC_CHECK_HEADERS([bits/stringfwd.h])
AC_CHECK_HEADERS([bits/concept_check.h])
AC_CHECK_HEADERS([bits/type_traits.h tr1/type_traits tr1/type_traits_fwd.h type_traits.h])
# among these, only sstream should be used, the rest are long gone
AC_CHECK_HEADERS([sstream strstream stringstream])
# can't include <tr1/boost_shared_ptr.h> directly
AC_CHECK_HEADERS([tr1/memory])
AC_CHECK_HEADERS([tr1/tuple tr1/array])

# compiler characteristic checks
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_C_RESTRICT
AC_TYPE_SIZE_T
AC_C_VOLATILE
AC_CHECK_TYPES([ptrdiff_t])
AC_CHECK_FUNCS([sqrt])
AC_CHECK_FUNCS([getopt])

# library functions
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC

# sizeof checks, useful for checking ILP model
AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(long long)
AC_CHECK_SIZEOF(float)
AC_CHECK_SIZEOF(double)
AC_CHECK_SIZEOF(long double)
AC_CHECK_SIZEOF(void*)
AC_CHECK_SIZEOF(size_t)

AC_LANG_RESTORE

# Lex/Yacc support
AM_PROG_LEX
AC_PROG_YACC

AC_ARG_VAR(LEX, [lexer/scanner, such as [f]lex])
AC_ARG_VAR(YACC, [parser generator, requires LALR(1), such as yacc/bison])

# Miscellaneous
AC_PROG_LN_S

# not so funny...
echo -n "checking for fang's sanity"
sleep 1; echo -n "."; sleep 1; echo -n "."; sleep 1; echo -n "."
sleep 1; echo " no"
# not enough sleep

# check whether or not bison is disguising as yacc (with bison -y)
# some parser builds in sub-directories will compile differently
# depending on which parser is used.  
AM_CONDITIONAL(HAVE_BISON, echo "$YACC" | grep -q bison)

if (echo "$YACC" | grep -q bison)
then
	AC_DEFINE(USING_BISON, 1, [Define to 1 if we're using bison.])
	AC_DEFINE(USING_YACC, 0, [Define to 1 if we're using yacc.])
else
	AC_DEFINE(USING_BISON, 0, [Define to 1 if we're using bison.])
	AC_DEFINE(USING_YACC, 1, [Define to 1 if we're using yacc.])
fi
# don't know what to do about byacc yet...

# AC_PROG_PERL
AC_CHECK_PROG([PERL], perl, perl)
AC_PATH_PROG([PERL_PATH], perl)

# documentation support
AC_CHECK_PROG([DOXYGEN], doxygen, doxygen)
AM_CONDITIONAL(HAVE_DOXYGEN, test -n "$DOXYGEN")

AC_CHECK_PROG([DOT], dot, dot)
AM_CONDITIONAL(HAVE_DOT, test -n "$DOT" )

AC_CHECK_PROG([LATEX], latex, latex)
AM_CONDITIONAL(HAVE_LATEX, test -n "$LATEX" )

AC_CHECK_PROG([PDFLATEX], pdflatex, pdflatex)
AM_CONDITIONAL(HAVE_PDFLATEX, test -n "$PDFLATEX" )

AC_CHECK_PROG([DVIPS], dvips, dvips)
AM_CONDITIONAL(HAVE_DVIPS, test -n "$DVIPS" )

AC_CHECK_PROG([BIBTEX], bibtex, bibtex)
AM_CONDITIONAL(HAVE_BIBTEX, test -n "$BIBTEX" )

AC_CHECK_PROG([MAKEINDEX], makeindex, makeindex)
AM_CONDITIONAL(HAVE_MAKEINDEX, test -n "$MAKEINDEX" )

# some substitution variables for the doxygen configuration
if test -n "$DOT"
then
	AC_SUBST(HAVE_DOT, "YES")
	AC_PATH_PROG([DOT_PATH], dot)
else
	AC_SUBST(HAVE_DOT, "NO")
fi

if test -n "$LATEX"
then
	AC_SUBST(HAVE_LATEX, "YES")
else
	AC_SUBST(HAVE_LATEX, "NO")
fi

if test -n "$PDFLATEX"
then
	AC_SUBST(HAVE_PDFLATEX, "YES")
else
	AC_SUBST(HAVE_PDFLATEX, "NO")
fi

# checking for old CAST tools for regression and backwards compatibility tests
# no big deal if they don't exist...
# NOTE: AM_CONDITIONAL is usable in generation of AC_CONFIG_FILES
#	just use @VAR_TRUE@ instead of the if ... version in automake
AC_CHECK_PROG([PRSIM], prsim, prsim)
AC_CHECK_PROG([CFLAT], cflat, cflat)
AM_CONDITIONAL(HAVE_PRSIM, test -n "$PRSIM" )
AM_CONDITIONAL(HAVE_CFLAT, test -n "$CFLAT" )

# shamelessly copied autoconf snippet from ngspige's configure.in
dnl	Option to include GNU readline support in ngspice CLI
dnl	Default: disabled.
dnl	Hope to see in the future readline replacement.
if test "$with_readline" != "yes"; then
	AC_MSG_RESULT(GNU readline disabled.)
else
	AC_MSG_RESULT(Checking for readline:)
	AC_CHECK_HEADERS([readline/readline.h readline/history.h],
		[AC_DEFINE(HAVE_GNUREADLINE,[],
			[Define if we have GNU readline])],
		[AC_MSG_ERROR(Couldn't find GNU readline headers.)])
	AC_SEARCH_LIBS(tputs,ncurses termcap,
		AC_DEFINE(HAVE_TERMCAP,[],
			[Define if we have ncurses or termcap]),
		AC_MSG_ERROR(Found neither ncurses or termcap))
	AC_CHECK_LIB(readline, readline,
		[LIBS="$LIBS -lreadline"],
		[AC_MSG_ERROR(Couldn't find readline libraries.)])
fi

dnl	Option to include BSD editline support in ngspice CLI
dnl	Default: disabled.
if test "$with_editline" != "yes"; then
	AC_MSG_RESULT(BSD editline disabled.)
else
	AC_MSG_RESULT(Checking for editline:)
	AC_CHECK_HEADERS([editline/readline.h],
		[AC_DEFINE([HAVE_BSDEDITLINE],[1],
			[Define to enable BSD editline])],
		[AC_MSG_ERROR(Couldn't find BSD editline headers.)])
	AC_SEARCH_LIBS(tputs,ncurses termcap,
		AC_DEFINE(HAVE_TERMCAP,[],
			[Define if we have ncurses or termcap]),
		AC_MSG_ERROR(Found neither ncurses or termcap))
	AC_CHECK_LIB(edit, readline,
		[LIBS="$LIBS -ledit"],
		[AC_MSG_ERROR(Couldn't find editline libraries.)],
		-lncurses )
fi
# end shameless copying from ngspice::configure.in

dnl first, create a useful variable substitution script for use in config.status
AC_CONFIG_FILES([config_subst.awk.sh:config/config_subst.awk],
[dollar=$
at=@
cat > config_subst.awk.sh <<EOF
#!$SHELL
awk -f $srcdir/config/config_subst.awk $dollar$at
EOF
chmod +x config_subst.awk.sh
])

AC_CONFIG_FILES([
	Makefile
	scripts/Makefile
	src/Makefile
	src/util/test/Makefile
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
	test/benchmarks/Makefile
	dox/Makefile
	dox/mk/Makefile
	dox/lang/Makefile
	dox/util/Makefile
	dox/objfmt/Makefile
	dox/man/Makefile
	dox/tutorial/Makefile
	hackt.doxygen.config
])
AC_CONFIG_FILES([src/util/test/test-expect.sh],
	[chmod +x src/util/test/test-expect.sh])
AC_CONFIG_FILES([test/artc-expect.sh],
	[chmod +x test/artc-expect.sh])
AC_CONFIG_FILES([test/artobj-diff.sh],
	[chmod +x test/artobj-diff.sh])
AC_CONFIG_FILES([test/artobjunroll-expect.sh],
	[chmod +x test/artobjunroll-expect.sh])
AC_CONFIG_FILES([test/hackt-create-expect.sh],
	[chmod +x test/hackt-create-expect.sh])
AC_CONFIG_FILES([test/hackt-alloc-expect.sh],
	[chmod +x test/hackt-alloc-expect.sh])
AC_CONFIG_FILES([test/hackt-cflat-expect.sh],
	[chmod +x test/hackt-cflat-expect.sh])
AC_CONFIG_FILES([test/test-series.sh],
	[chmod +x test/test-series.sh])

dnl no input file needsd, just referencing arbitrary existing file
AC_CONFIG_FILES([configdate.h:config/macro-value.h.in],
	[date=`./config_subst.awk.sh -v var=DATE config.status`
	sed 's/MACRO/CONFIGDATE/g' $srcdir/config/macro-value.h.in | \
		sed "s/VALUE/\"`$date`\"/g" > configdate.h
	])

AC_CONFIG_FILES([src/buildhost.h:config/macro-value.h.in],
	[host=`./config_subst.awk.sh -v var=host config.status`
	build=`./config_subst.awk.sh -v var=build config.status`
	sed 's/MACRO/CONFIG_HOST/g' $srcdir/config/macro-value.h.in | \
		sed "s/VALUE/\"$host\"/g" > src/buildhost.h
	sed 's/MACRO/CONFIG_BUILD/g' $srcdir/config/macro-value.h.in | \
		sed "s/VALUE/\"$build\"/g" >> src/buildhost.h
	])

AC_CONFIG_FILES([src/cxx_version.h:config/macro-value.h.in],
	[cxx=`./config_subst.awk.sh -v var=CXX config.status`
	sed 's/MACRO/CXX_VERSION/g' $srcdir/config/macro-value.h.in | \
		sed "s/VALUE/\"`$cxx --version | head -n 1`\"/g" > src/cxx_version.h
	])

AC_CONFIG_FILES([src/cxxflags.h:config/macro-value.h.in],
	[cxxflags=`./config_subst.awk.sh -v var=CXXFLAGS config.status`
	sed 's/MACRO/CONFIG_CXXFLAGS/g' $srcdir/config/macro-value.h.in | \
		sed "s/VALUE/\"$cxxflags\"/g" > src/cxxflags.h
	])

AC_CONFIG_FILES([src/lexer/lex_version.h:config/macro-value.h.in],
	[lex=`./config_subst.awk.sh -v var=LEX config.status`
	sed 's/MACRO/LEX_VERSION/g' $srcdir/config/macro-value.h.in | \
		sed "s/VALUE/\"`$lex --version | head -n 1`\"/g" > src/lexer/lex_version.h
	])

dnl traditional yacc has no version flag :(
AC_CONFIG_FILES([src/parser/yacc_version.h:config/macro-value.h.in],
	[yacc=`./config_subst.awk.sh -v var=YACC config.status`
	version="none"
	if ( ./config_subst.awk.sh -v var=HAVE_BISON_TRUE config.status | grep -q "#" )
	then
		version=`which $yacc | head -n 1`
	else
		version=`$yacc --version 2>&1 | head -n 1`
	fi
	sed 's/MACRO/YACC_VERSION/g' $srcdir/config/macro-value.h.in | \
		sed "s|VALUE|\"$version\"|g" > src/parser/yacc_version.h
	])

AC_CONFIG_FILES([src/parser/hackt-parse.yy:src/parser/hackt-parse.yy.in],
	[if ( ./config_subst.awk.sh -v var=HAVE_BISON_TRUE config.status | grep -q "#" )
	then
		sed '/%pure.parser/d' $srcdir/src/parser/hackt-parse.yy.in > \
			src/parser/hackt-parse.yy
	else
		cp $srcdir/src/parser/hackt-parse.yy.in src/parser/hackt-parse.yy
	fi
	])

AC_OUTPUT

