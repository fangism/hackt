# "configure.ac"
# Process this file with "autoconf" to generate the "configure" script.
# (Prior to 2005/06/19, this file was "configure.in", 
#	in case one should want to trace back the CVS revision history.)
#
#	$Id: configure.ac,v 1.4.2.1 2005/08/05 21:08:08 fang Exp $

# Hint: Try running "autoscan" for suggestions on what should go in here.  
# Results of autoscan appear in "configure.scan".  

AC_PREREQ(2.59)
# using art++ results in art-- :(
AC_INIT(hackt, 0.1.4-devel-20050804, fangism@users.sourceforge.net)
AC_CONFIG_AUX_DIR(config)
AM_CONFIG_HEADER(config.h)
AC_CONFIG_SRCDIR([src/main/main_funcs.cc])

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
AC_CHECK_PROG([GREP], grep, grep)
AC_CHECK_PROG([ECHO], echo, echo)
AC_CHECK_PROG([HEAD], head, head)
AC_CHECK_PROG([TAIL], tail, tail)
# AC_CHECK_PROG([PWD], pwd, pwd)
#	checking for PWD doesn't work as expected...
AC_CHECK_PROG([SED], sed, sed)
AC_CHECK_PROG([TOUCH], touch, touch)
AC_DEFINE_UNQUOTED(HACKT_BUILD_DATE, "`$DATE`", [Define the build date.])

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
AM_PROG_CC_C_O
AC_ISC_POSIX

# additional header checks
AC_CHECK_HEADERS([stddef.h stdlib.h string.h unistd.h getopt.h])
AC_CHECK_HEADERS(pthread.h)
# AC_CHECK_HEADERS([cstddef cstdlib cstdio cstring cassert cmath])

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

AC_CONFIG_FILES([
	Makefile
	src/Makefile
	src/util/test/Makefile
	src/util/test/test-expect.sh
	test/Makefile
	test/artc-expect.sh
	test/artobj-diff.sh
	test/artobjunroll-expect.sh
	test/test-series.sh
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
	dox/Makefile
	dox/mk/Makefile
	dox/lang/Makefile
	dox/util/Makefile
	dox/objfmt/Makefile
	hackt.doxygen.config
	])

AC_OUTPUT

# make configure-generated scripts executable
chmod +x src/util/test/test-expect.sh
chmod +x test/test-series.sh
chmod +x test/artc-expect.sh
chmod +x test/artobj-diff.sh
chmod +x test/artobjunroll-expect.sh

