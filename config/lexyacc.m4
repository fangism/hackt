dnl "config/lexyacc.m4"
dnl	$Id: lexyacc.m4,v 1.5 2007/11/01 23:59:39 fang Exp $
dnl This file contains autoconf macros related to lex and yacc, 
dnl including bison.  
dnl These may be slightly more specific to the HACKT project.
dnl

dnl @synopsis HACKT_ARG_VAR_LEX
dnl
dnl Enables LEX as a configurable variable.  
dnl
dnl @category InstalledPackages
dnl @version 2007-11-01
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([HACKT_ARG_VAR_LEX],
[AC_REQUIRE([AM_PROG_LEX])
AC_ARG_VAR(LEX, [lexer/scanner, such as [f]lex])
if test "$LEX"
then
dnl now test for lex features
cat > conftest.l <<ACEOF
/* line counter, example take from man page */
%{
int num_lines = 0, num_chars = 0;
%}
%%
\n	{ ++num_lines; ++num_chars; }
.	{ ++num_chars; }
%%

ACEOF
dnl I need to define an AC_LEX_IFELSE(PROGRAM, [TRUE-ACTION], [FALSE-ACTION])
dnl ac_lex='$LEX conftest.l'
ac_compile_lex='$CC -c $CFLAGS $CPPFLAGS $ac_cv_prog_lex_root.c >&5'
dnl ac_link_lex='$CC ley.yy.$ac_objext -o conftest$ac_exeext $CFLAGS $CPPFLAGS $LDFLAGS >&5'
if $LEX conftest.l && eval "$ac_compile_lex"
dnl && eval "$ac_link_lex"
then
	dnl perform some more tests...
	dnl check for lineno
AC_CACHE_CHECK([whether generated lexer has yylineno],
[fang_cv_lex_yylineno], [
	saved_LDFLAGS="$LDFLAGS"
	LDFLAGS="ac_cv_prog_lex_root.$ac_objext $LDFLAGS"
	AC_LINK_IFELSE(
		AC_LANG_PROGRAM([
			#include <stdio.h>
			extern int yylineno;
		], [yylex(); printf("%d\n", yylineno);]),
		[fang_cv_lex_yylineno=yes],
		[fang_cv_lex_yylineno=no]
	)
	LDFLAGS="$saved_LDFLAGS"
])
	if test "$fang_cv_lex_yylineno" = yes
	then
	AC_DEFINE(LEXER_HAS_YYLINENO, 1, [Define to 1 of lexer has yylineno.])
	fi
dnl check for buffer stack
AC_CACHE_CHECK([whether generated lexer has yy_buffer_stack],
[fang_cv_lex_yy_buffer_stack], [
	saved_LDFLAGS="$LDFLAGS"
	LDFLAGS="ac_cv_prog_lex_root.$ac_objext $LDFLAGS"
	AC_LINK_IFELSE(
		AC_LANG_PROGRAM([
			#include <stdio.h>
			extern void* yy_buffer_stack;
		], [yylex(); printf("%p\n", yy_buffer_stack);]),
		[fang_cv_lex_yy_buffer_stack=yes],
		[fang_cv_lex_yy_buffer_stack=no]
	)
	LDFLAGS="$saved_LDFLAGS"
])
	if test "$fang_cv_lex_yy_buffer_stack" = yes
	then
	AC_DEFINE(LEXER_HAS_BUFFER_STACK, 1, [Define to 1 of lexer has buffer-stack.])
	fi
dnl check for yylex_destroy
AC_CACHE_CHECK([whether generated lexer has yylex_destroy],
[fang_cv_lex_yylex_destroy], [
	saved_LDFLAGS="$LDFLAGS"
	LDFLAGS="ac_cv_prog_lex_root.$ac_objext $LDFLAGS"
	AC_LINK_IFELSE(
		AC_LANG_PROGRAM([
			#include <stdio.h>
			extern void yylex_destroy();
		], [yylex(); yylex_destroy();]),
		[fang_cv_lex_yylex_destroy=yes],
		[fang_cv_lex_yylex_destroy=no]
	)
	LDFLAGS="$saved_LDFLAGS"
])
	if test "$fang_cv_lex_yylex_destroy" = yes
	then
	AC_DEFINE(LEXER_HAS_YYLEX_DESTROY, 1, [Define to 1 of lexer has yylex_destroy.])
	fi
else
	AC_MSG_NOTICE([lex (compile) doesn't seem to work!])
fi
dnl end if $LEX conftest.l
rm -f conftest.l $ac_cv_prog_lex_root.* conftest$ac_exeext
fi
dnl end if test $LEX
])dnl

dnl @synopsis HACKT_ARG_VAR_YACC
dnl
dnl Enables YACC as a configurable variable.  
dnl This usually picks up bison by default if found, 
dnl but can be manually overridden.
dnl
dnl @category InstalledPackages
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([HACKT_ARG_VAR_YACC],
[AC_REQUIRE([AC_PROG_YACC])
AC_ARG_VAR(YACC, [parser generator, requires LALR(1), such as yacc/bison])
])dnl

dnl @synopsis HACKT_LEX_VERSION
dnl
dnl Detects and records the LEX version as LEX_VERSION.  
dnl
dnl @category InstalledPackages
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
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
])dnl

dnl @synopsis HACKT_AM_CONDITIONAL_HAVE_YACC
dnl
dnl Defines one of {HAVE_BISON, HAVE_BYACC, HAVE_YACC} to be true for automake.
dnl check whether or not bison is disguising as yacc (with bison -y)
dnl some parser builds in sub-directories will compile differently
dnl depending on which parser is used.  
dnl
dnl @category InstalledPackages
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([HACKT_AM_CONDITIONAL_HAVE_YACC],
[AC_REQUIRE([AC_PROG_YACC])
dnl reminder: AM_CONDITIONALs must be defined unconditionally
AM_CONDITIONAL(HAVE_BISON, echo "$YACC" | grep -q bison)
AM_CONDITIONAL(HAVE_BYACC, echo "$YACC" | grep -q byacc)
AM_CONDITIONAL(HAVE_YACC, echo "$YACC" | grep -v byacc | grep -q yacc)
])dnl

dnl @synopsis HACKT_YACC_PURE
dnl
dnl AC_SUBST the variable YACC_PURE_PARSER for use in yacc sources
dnl if the %pure-parser directive is supported.
dnl
dnl @category InstalledPackages
dnl @version 2006-08-29
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([HACKT_YACC_PURE_PARSER],
[AC_REQUIRE([AC_PROG_YACC])
case $YACC in
	dnl (
	*bison* ) YACC_PURE_PARSER=%pure_parser ;;
	dnl (
	*)	YACC_PURE_PARSER="" ;;
esac
AC_SUBST(YACC_PURE_PARSER)
])dnl


dnl @synopsis HACKT_YACC_VERSION
dnl
dnl defines YACC_VERSION or BISON_VERSION with a version string if possible
dnl TODO: check whether or not bison/yacc works on a basic file
dnl this must be consistent with the above
dnl there might be a better way to do this...
dnl
dnl @category InstalledPackages
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
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
])dnl


