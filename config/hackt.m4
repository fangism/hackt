dnl "config/hackt.m4"
dnl	$Id: hackt.m4,v 1.1.2.1 2006/02/22 08:09:41 fang Exp $
dnl
dnl This file is for autoconf macros specific to HACKT.
dnl General-purpose macros should be based in other m4 files.  
dnl

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


