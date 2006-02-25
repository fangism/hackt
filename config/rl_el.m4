dnl "config/rl_el.m4"
dnl	$Id: rl_el.m4,v 1.2 2006/02/25 04:55:00 fang Exp $
dnl Readline and Editline support for the utility library used by hackt.
dnl This is not only specific to hackt, so we place these macros here.  
dnl
dnl TODO: change the AC_ARG_WITH macros to use PATH arguments.  
dnl

dnl ripped from ngspice's configure.in and adapted for own uses
dnl TODO: rip the library and header checks too, to be invoked later
dnl It would be a good idea to see how they handled compatiblity between the two
dnl
dnl --with-readline:  Includes GNU readline support into CLI.  Default is "no".
dnl Including readline into ngspice is a violation of GPL license. It's use
dnl is discouraged for the hackt project.
dnl
AC_DEFUN([_AC_ARG_WITH_READLINE],
[
dnl AC_MSG_CHECKING([whether readline is requested])
AC_ARG_WITH(readline,
[[  --with-readline[=PATH]  Enable GNU readline support for CLI.  Default=no.]]
dnl	AS_HELP_STRING([[--with-readline[=yes/no]]],
dnl		[Enable GNU readline support for CLI.  Default=no.])
)
if test x"$with_readline" != xno && test "$with_readline" ; then
	AC_MSG_WARN([
	This project license is not GPL-compatible to qualify for using GNU
	readline.  Please consider linking against BSD editline instead.])
fi
])

dnl
dnl --with-editline:  Includes BSD Editline support into CLI.  Default is "no".
dnl
AC_DEFUN([_AC_ARG_WITH_EDITLINE],
[
dnl AC_MSG_CHECKING([whether editline is requested])
AC_ARG_WITH(editline, 
[[  --with-editline[=PATH]  Enable BSD editline support for CLI.  Default=no.]]
dnl	AS_HELP_STRING([[--with-editline[=yes/no]]],
dnl		[Enable BSD editline support for CLI.  Default=no.])
)
])

dnl
dnl readline and editline cannot both be enabled
dnl
AC_DEFUN([AC_ARG_WITH_READLINE_EDITLINE],
[AC_REQUIRE([_AC_ARG_WITH_READLINE])
AC_REQUIRE([_AC_ARG_WITH_EDITLINE])
AC_REQUIRE([HACKT_ARG_WITH_NCURSES])
if test x"$with_readline" != xno && test "$with_readline" ; then
	if test x"$with_editline" != xno && test "$with_editline" ; then
		AC_MSG_ERROR(Readline and editline cannot both be enabled!)
	fi
fi
])

dnl
dnl Checks for readline usability.
dnl
AC_DEFUN([AC_CHECK_READLINE],
[AC_REQUIRE([AC_ARG_WITH_READLINE_EDITLINE])
AC_REQUIRE([AC_PROG_CC])
dnl this test can be in C or C++ mode, doesn't matter
if test -z "$with_readline" || test x"$with_readline" = xno ; then
        AC_MSG_RESULT(GNU readline disabled.)
else
rl_ldpath=
rl_include=
if test "$with_readline" != yes ; then
	dnl we take the PATH argument and use it for testing
	rl_ldpath="-L$with_readline/lib"
	rl_include="-I$with_readline/include"
fi
saved_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $rl_include"
        AC_MSG_RESULT(Checking for readline:)
        AC_CHECK_HEADERS([readline/readline.h readline/history.h],
                [AC_DEFINE(HAVE_GNUREADLINE,[], 
                        [Define if we have GNU readline])
		dnl saved_CPPFLAGS="$CPPFLAGS"
		RL_INCLUDE="$rl_include"],
                [AC_MSG_ERROR(Couldn't find GNU readline headers.)])
	dnl depends on terminal library support
	if test x"$ac_cv_search_tputs" != xno ; then
		:
		dnl remember to link in proper order
		dnl LIBS="$ac_cv_search_tputs $LIBS"
	else
	AC_MSG_ERROR([Found neither ncurses or termcap, needed by readline])
	fi
	saved_LDFLAGS="$LDFLAGS"
	LDFLAGS="$LDFLAGS $rl_ldpath"
        AC_CHECK_LIB(readline, readline,
                [
		dnl LIBS="-lreadline $LIBS"
		dnl saved_LDFLAGS="$LDFLAGS"
		RL_LDPATH="$rl_ldpath"
		RL_LIB="-lreadline"],
                [AC_MSG_ERROR(Couldn't find readline libraries in LDFLAGS paths.)],
	dnl extra library argument to link during link test
	$NCURSES_LIB )
	LDFLAGS="$saved_LDFLAGS"
CPPFLAGS="$saved_CPPFLAGS"
fi
dnl these are set only oif libs/headers are found
AC_SUBST(RL_INCLUDE)
AC_SUBST(RL_LIB)
AC_SUBST(RL_LDPATH)
])

dnl
dnl Same check for editline usability
dnl
AC_DEFUN([AC_CHECK_EDITLINE],
[AC_REQUIRE([AC_ARG_WITH_READLINE_EDITLINE])
AC_REQUIRE([AC_PROG_CC])
if test -z "$with_editline" || test x"$with_editline" = "xno"; then
	AC_MSG_RESULT(BSD editline disabled.)
else
el_ldpath=
el_include=
if test "$with_editline" != yes ; then
	dnl we take the PATH argument and use it for testing
	el_ldpath="-L$with_editline/lib"
	el_include="-I$with_editline/include"
fi
saved_CPPFLAGS="$CPPFLAGS"
saved_LDFLAGS="$LDFLAGS"
CPPFLAGS="$CPPFLAGS $el_include"
LDFLAGS="$LDFLAGS $el_ldpath"
	AC_MSG_RESULT(Checking for editline:)
	AC_CHECK_HEADERS([editline/readline.h],
		[AC_DEFINE([HAVE_BSDEDITLINE],[1],
			[Define to enable BSD editline])
		dnl saved_CPPFLAGS="$CPPFLAGS"
		EL_INCLUDE="$el_include"],
		[AC_MSG_ERROR(Couldn't find BSD editline headers.)])
	dnl depends on terminal library support
	if test x"$ac_cv_search_tputs" != xno ; then
		:
		dnl remember to link in proper order
		dnl LIBS="$ac_cv_search_tputs $LIBS"
	else
	AC_MSG_ERROR([Found neither ncurses or termcap, needed by editline])
	fi
	AC_CHECK_LIB(edit, readline,
		[
		dnl LIBS="-ledit $LIBS"
		dnl saved_LDFLAGS="$LDFLAGS"
		EL_LDPATH="$el_ldpath"
		EL_LIB="-ledit"],
		[AC_MSG_ERROR(Couldn't find editline libraries in LDFLAGS paths.)],
	dnl extra library argument to link during link test
	$NCURSES_LIB )
LDFLAGS="$saved_LDFLAGS"
CPPFLAGS="$saved_CPPFLAGS"
fi
dnl these are set only oif libs/headers are found
AC_SUBST(EL_INCLUDE)
AC_SUBST(EL_LIB)
AC_SUBST(EL_LDPATH)
])

dnl
dnl check for const char* in prototype
dnl
AC_DEFUN([AC_CHECK_READLINE_ARG_CONST],
[AC_REQUIRE([AC_CHECK_READLINE])
AC_REQUIRE([AC_CHECK_EDITLINE])
AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
dnl if test "$ac_cv_header_readline_readline_h" = "yes" || \
dnl     test "$ac_cv_header_editline_readline_h" = "yes"
if test "$with_readline" = "yes" || test "$with_editline" = "yes"
then
AC_MSG_CHECKING([if readline()'s prompt argument is const char*])
dnl need to have warnings enabled and turned to errors here
AC_LANG_PUSH(C++)
save_CXXFLAGS=$CXXFLAGS
CXXFLAGS="$save_CXXFLAGS $ANAL_FLAGS"
	AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([
	#include <cstdio>
	#if defined(HAVE_GNUREADLINE)
	#include <readline/readline.h>
	#elif defined(HAVE_BSDEDITLINE)
	#include <editline/readline.h>
	#endif
	],[[[const char null[] = "foo"; readline(null) ]]]),
	dnl action on success
	[AC_MSG_RESULT([yes])
	AC_DEFINE(READLINE_PROMPT_CONST, 1,
		[True if readline's argument is const char*])],
	dnl action on failure
	[AC_MSG_RESULT([no (you suck!)])
	AC_DEFINE(READLINE_PROMPT_CONST, 0,
		[True if readline's argument is const char*])
	])
dnl restore compile flags
CXXFLAGS=$save_CXXFLAGS
AC_LANG_POP(C++)
fi
])


