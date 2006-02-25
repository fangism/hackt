dnl "config/lexyacc.m4"
dnl	$Id: lexyacc.m4,v 1.2 2006/02/25 04:54:59 fang Exp $
dnl This file contains autoconf macros related to lex and yacc, 
dnl including bison.  
dnl These may be slightly more specific to the HACKT project.
dnl

dnl
dnl Enables LEX as a configurable variable.  
dnl
AC_DEFUN([HACKT_ARG_VAR_LEX],
[AC_REQUIRE([AM_PROG_LEX])
AC_ARG_VAR(LEX, [lexer/scanner, such as [f]lex])
])

dnl
dnl Enables YACC as a configurable variable.  
dnl This usually picks up bison by default if found, 
dnl but can be manually overridden
dnl
AC_DEFUN([HACKT_ARG_VAR_YACC],
[AC_REQUIRE([AC_PROG_YACC])
AC_ARG_VAR(YACC, [parser generator, requires LALR(1), such as yacc/bison])
])

dnl
dnl Detects and records the LEX version as LEX_VERSION.  
dnl
AC_DEFUN([HACKT_LEX_VERSION],
[AC_REQUIRE([AM_PROG_LEX])
dnl unfortunately, lex is often IDENTICAL to flex...
case $LEX in
	*flex* )
		AC_DEFINE(USING_FLEX, 1, [Define to 1 if we're using flex.])
		AC_DEFINE(USING_LEX, 0, [Define to 1 if we're using lex.])
		;;
	*lex* )
		AC_DEFINE(USING_FLEX, 0, [Define to 1 if we're using lex.])
		AC_DEFINE(USING_LEX, 1, [Define to 1 if we're using lex.])
		;;
	* ) AC_MSG_ERROR([No lexer-generator found.]) ;;
esac
LEX_VERSION=`$LEX --version | head -n 1`
AC_SUBST(LEX_VERSION)
])


dnl
dnl Defines one of {HAVE_BISON, HAVE_BYACC, HAVE_YACC} to be true for automake.
dnl check whether or not bison is disguising as yacc (with bison -y)
dnl some parser builds in sub-directories will compile differently
dnl depending on which parser is used.  
dnl
AC_DEFUN([HACKT_AM_CONDITIONAL_HAVE_YACC],
[AC_REQUIRE([AC_PROG_YACC])
dnl reminder: AM_CONDITIONALs must be defined unconditionally
AM_CONDITIONAL(HAVE_BISON, echo "$YACC" | grep -q bison)
AM_CONDITIONAL(HAVE_BYACC, echo "$YACC" | grep -q byacc)
AM_CONDITIONAL(HAVE_YACC, echo "$YACC" | grep -v byacc | grep -q yacc)
])

dnl
dnl defines YACC_VERSION or BISON_VERSION with a version string if possible
dnl TODO: check whether or not bison/yacc works on a basic file
dnl this must be consistent with the above
dnl there might be a better way to do this...
dnl
AC_DEFUN([HACKT_YACC_VERSION],
[AC_REQUIRE([AC_PROG_YACC])
case $YACC in
	dnl (
	*bison* )
		dnl echo "GOT bison"
		AC_DEFINE(USING_BISON, 1, [Define to 1 if we're using bison.])
		AC_DEFINE(USING_BYACC, 0, [Define to 1 if we're using byacc.])
		AC_DEFINE(USING_YACC, 0, [Define to 1 if we're using yacc.])
		dnl Now would be a good place to check for version
		dnl since bison is known to have significant variations.  
		YACC_VERSION=`$YACC --version 2>&1 | head -n 1`
		[BISON_VERSION=`echo x$YACC_VERSION | sed 's/[^.0-9]//g'`]
		AC_MSG_NOTICE([found bison version $BISON_VERSION.])
		dnl known version checks
dnl the below code doesn't work...
dnl	if { awk 'END{ if($BISON_VERSION +0.0<=1.35) exit 1;}' </dev/null;}
dnl	then 
dnl dnl		if { awk 'END{if($BISON_VERSION +0.0<=1.28) exit 1;}' </dev/null;}
dnl dnl		then AC_MSG_ERROR([
dnl dnl Your bison (<= 1.28) is known to fail with this project.
dnl dnl Please upgrade to 1.875 or higher, or use traditional yacc.])
dnl dnl		else
dnl			AC_MSG_WARN([
dnl	Your bison (<= 1.35) is not officially supported by this project.  
dnl	Consider upgrading to 1.875 or higher, or use traditional yacc.])
dnl dnl		fi
dnl	fi
		;;
	dnl (
	*byacc* )
		dnl echo "GOT byacc"
		AC_DEFINE(USING_BISON, 0, [Define to 1 if we're using bison.])
		AC_DEFINE(USING_BYACC, 1, [Define to 1 if we're using byacc.])
		AC_DEFINE(USING_YACC, 0, [Define to 1 if we're using yacc.])
		YACC_VERSION=`which $YACC | head -n 1`
		;;
	dnl (
	*yacc* )
		dnl echo "GOT yacc"
		AC_DEFINE(USING_BISON, 0, [Define to 1 if we're using bison.])
		AC_DEFINE(USING_BYACC, 0, [Define to 1 if we're using byacc.])
		AC_DEFINE(USING_YACC, 1, [Define to 1 if we're using yacc.])
		dnl traditional yacc has no version flag :S
		YACC_VERSION=`which $YACC | head -n 1`
		;;
	dnl (
	* ) AC_MSG_ERROR([No parser-generator found.]) ;;
esac
AC_SUBST(YACC_VERSION)
])


