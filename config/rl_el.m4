dnl "config/rl_el.m4"
dnl	$Id: rl_el.m4,v 1.1.2.1 2006/02/23 04:36:13 fang Exp $
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
	AS_HELP_STRING([--with-readline[=yes/no]],
		[Enable GNU readline support for CLI.  Default=no.]))
if test x"$with_readline" != xno; then
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
	AS_HELP_STRING([--with-editline[=yes/no]],
		[Enable BSD editline support for CLI.  Default=no.]))
])

dnl
dnl readline and editline cannot both be enabled
dnl
AC_DEFUN([AC_ARG_WITH_READLINE_EDITLINE],
[AC_REQUIRE([_AC_ARG_WITH_READLINE])
AC_REQUIRE([_AC_ARG_WITH_EDITLINE])
if test x"$with_readline" = xyes; then
	if test x"$with_editline" = xyes; then
		AC_MSG_ERROR(Readline and editline cannot both be enabled!)
	fi
fi
])

dnl
dnl Checks for readline usability.
dnl TODO: use PATH argument for search prefix.  
dnl
AC_DEFUN([AC_CHECK_READLINE],
[AC_REQUIRE([AC_ARG_WITH_READLINE_EDITLINE])
AC_REQUIRE([AC_PROG_CC])
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
                [AC_MSG_ERROR(Couldn't find readline libraries.  Try passing extra LDFLAGS.)])
fi
])

dnl
dnl Same check for editline usability
dnl TODO: use PATH argument
dnl
AC_DEFUN([AC_CHECK_EDITLINE],
[AC_REQUIRE([AC_ARG_WITH_READLINE_EDITLINE])
AC_REQUIRE([AC_PROG_CC])
if test x"$with_editline" != "xyes"; then
	AC_MSG_RESULT(BSD editline disabled.)
else
	AC_MSG_RESULT(Checking for editline:)
	AC_CHECK_HEADERS([editline/readline.h],
		[AC_DEFINE([HAVE_BSDEDITLINE],[1],
			[Define to enable BSD editline])],
		[AC_MSG_ERROR(Couldn't find BSD editline headers.)])
	AC_SEARCH_LIBS(tputs, ncurses termcap,
		AC_DEFINE(HAVE_TERMCAP,[],
			[Define if we have ncurses or termcap]),
		AC_MSG_ERROR(Found neither ncurses or termcap))
	AC_CHECK_LIB(edit, readline,
		[LIBS="$LIBS -ledit"],
		[AC_MSG_ERROR(Couldn't find editline libraries.  Try passing extra LDFLAGS.)],
		-lncurses
	)
fi
])

dnl check for const char* in prototype


