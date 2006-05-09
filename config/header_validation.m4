dnl "config.header_validation.m4"
dnl	$Id: header_validation.m4,v 1.3 2006/05/09 05:39:18 fang Exp $
dnl autoconf macros dealing with header validation checks
dnl	related files:
dnl	src/Make.hchk-head
dnl	src/Make.hchk-tail
dnl

dnl @synopsis FANG_ARG_ENABLE_HEADER_VALIDATION
dnl
dnl enable or disable anal header tests in src/Make.hchk-tail.am
dnl This conditionally sets the AM_CONDITIONAL: ENABLE_HEADER_VALIDATION
dnl
dnl @category Cxx
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_ARG_ENABLE_HEADER_VALIDATION],
[AC_MSG_CHECKING([whether header validation is desired])
AC_ARG_ENABLE(header_validation,
dnl help text
AC_HELP_STRING([--disable-header-validation],
[Perform source header validation before compiling.
Disable to speed-up release builds [(default=enabled)]]),
dnl if given
[if test x"$enable_header_validation" = x"yes"
then    AC_MSG_RESULT([yes])
else    AC_MSG_RESULT([no])
fi
],
dnl if not given 
AC_MSG_RESULT([yes (default)])
)
AM_CONDITIONAL(ENABLE_HEADER_VALIDATION, 
test -z "$enable_header_validation" || test x"$enable_header_validation" = xyes)
])dnl

