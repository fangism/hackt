dnl "config/hackt.m4"
dnl	$Id: hackt.m4,v 1.1.2.3 2006/02/23 06:30:02 fang Exp $
dnl
dnl This file is for autoconf macros specific to HACKT.
dnl General-purpose macros should be based in other m4 files.  
dnl

dnl
dnl For this project, we prefer to keep libtool --silent by default,
dnl however, one can use this to enable verbose mode.
dnl NOTE: if desired this could be generalized to pass other flags.
dnl
AC_DEFUN([HACKT_ARG_ENABLE_LIBTOOL_VERBOSE],
[AC_REQUIRE([AC_PROG_LIBTOOL])
AC_ARG_ENABLE(libtool-verbose,
	AS_HELP_STRING([--enable-libtool-verbose],
		[Standard (non-silent) libtool invocations (default=disabled)])
	)
if test x"$enable_libtool_verbose" != "xyes" ; then
	dnl default --silent
	LIBTOOL="$LIBTOOL --silent"
fi
])

dnl testing the AC_ARG_ENABLE autoconf feature
dnl god-mode doesn't actually do anything... yet
AC_DEFUN([HACKT_ARG_ENABLE_GOD_MODE],
[AC_MSG_CHECKING([whether divine powers are bestowed upon thee])
AC_ARG_ENABLE(god_mode,
dnl help text
AS_HELP_STRING([--enable-god-mode],
	[Powers fit for only immortals [(default=disabled)]]),
dnl if given
	[if test "$enable_god_mode" = "yes"
	then    AC_MSG_RESULT([you wish])
	else    AC_MSG_RESULT([no])
	fi
	],
dnl if not given
	[AC_MSG_RESULT([no (default)])]
)])


dnl
dnl Just for fun. 
dnl enable or disable fun modules
dnl
AC_DEFUN([HACKT_ARG_ENABLE_FUN],
[AC_MSG_CHECKING([how bored we are])
AC_ARG_ENABLE(fun,
	AS_HELP_STRING([--enable-fun],
		[Miscellanous fun modules. (default=disabled)]),
	AM_CONDITIONAL(WANT_LIBMISCFUN, test x"$enable_fun" = "xyes"),
	AM_CONDITIONAL(WANT_LIBMISCFUN, test ! "yes")
)
if test x"$enable_fun" = "xyes"
then    AC_MSG_RESULT([quite])
	AC_DEFINE(WANT_TO_HAVE_FUN, 1, [Conditional inclusion of fun modules.])
else    AC_MSG_RESULT([not yet])
	AC_DEFINE(WANT_TO_HAVE_FUN, 0, [Conditional inclusion of fun modules.])
fi
])

dnl
dnl This macro doesn't really belong here, whatever...
dnl
AC_DEFUN([HACKT_ARG_ENABLE_UNIVERSAL_BINARY],
[AC_MSG_CHECKING([whether universal binaries are requested])
AC_ARG_ENABLE(fat_binary,
	AS_HELP_STRING([--enable-fat-binary],
		[Compile programs for multiple architectures.]),
	[AC_MSG_RESULT([yes, but...])
	dnl AC_CHECK_PROG(lipo)
	dnl check whether or not compiler support multiple -arch flags
	AC_MSG_NOTICE([Support for this is forthcoming for Darwin.])
	dnl if enabled, then disable dynamic dependence-tracking
	],
	[AC_MSG_RESULT([no])]
)
])

dnl
dnl Sleep is evil.
dnl
AC_DEFUN([AC_PROG_SLEEP],
[AC_CHECK_PROG([SLEEP], sleep, sleep)
if test ! -n "$SLEEP"
then
	AC_MSG_ERROR([How do you expect to work without sleep?])
fi
AC_MSG_CHECKING([for *enough* sleep])
AC_MSG_RESULT([maybe])
])

dnl
dnl Sanity check.
dnl
AC_DEFUN([HACKT_CHECK_SANITY],
[AC_REQUIRE([AC_PROG_SLEEP])
dnl not so funny...
AC_MSG_CHECKING([whether fang is sane])
dnl sleep 1; echo -n "."; sleep 1; echo -n "."; sleep 1; echo -n "." sleep 1; 
$SLEEP 3
AC_MSG_RESULT([no]);
dnl not enough sleep
])

