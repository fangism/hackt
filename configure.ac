# "configure.ac"
# Process this file with "autoconf" to generate the "configure" script.
# (Prior to 2005/06/19, this file was "configure.in", 
#	in case one should want to trace back the CVS revision history.)
#
#	$Id: configure.ac,v 1.1 2005/06/20 19:47:31 fang Exp $

# Hint: Try running "autoscan" for suggestions on what should go in here.  
# Results of autoscan appear in "configure.scan".  

AC_PREREQ(2.59)
# using art++ results in art-- :(
AC_INIT(artxx, 0.1.4, fangism@users.sourceforge.net)
AC_CONFIG_AUX_DIR(config)
AM_CONFIG_HEADER(config.h)
AC_CONFIG_SRCDIR([src/art_main.cc])

AC_CANONICAL_HOST

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

# documentation support
AC_CHECK_PROG([DOXYGEN], doxygen, doxygen)
AM_CONDITIONAL(HAVE_DOXYGEN, test "$DOXYGEN" = doxygen)

AC_CHECK_PROG([DOT], dot, dot)
AM_CONDITIONAL(HAVE_DOT, test "$DOT" = dot)

AC_CHECK_PROG([LATEX], latex, latex)
AM_CONDITIONAL(HAVE_LATEX, test "$LATEX" = latex)

AC_CHECK_PROG([PDFLATEX], pdflatex, pdflatex)
AM_CONDITIONAL(HAVE_PDFLATEX, test "$PDFLATEX" = pdflatex)

AC_CHECK_PROG([DVIPS], dvips, dvips)
AM_CONDITIONAL(HAVE_DVIPS, test "$DVIPS" = dvips)

AC_CHECK_PROG([BIBTEX], bibtex, bibtex)
AM_CONDITIONAL(HAVE_BIBTEX, test "$BIBTEX" = bibtex)

AC_CHECK_PROG([MAKEINDEX], makeindex, makeindex)
AM_CONDITIONAL(HAVE_MAKEINDEX, test "$MAKEINDEX" = makeindex)

AC_CONFIG_FILES([
	Makefile
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
	dox/Makefile
	dox/mk/Makefile
	dox/lang/Makefile
	dox/util/Makefile
	dox/objfmt/Makefile
	])

AC_OUTPUT

