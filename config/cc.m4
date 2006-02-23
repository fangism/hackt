dnl "config/cc.m4"
dnl	$Id: cc.m4,v 1.1.2.1 2006/02/23 06:30:02 fang Exp $
dnl General configure macros for detecting characteristics of the C compiler.
dnl

dnl
dnl Checks for common underlying typedefs for the C-stdio FILE type.  
dnl The purpose of this is to be able to forward declare a FILE*
dnl without having to include <stdio.h>
dnl
AC_DEFUN([AC_TYPEDEF_FILE],
[AC_REQUIRE([AC_PROG_CC])
AC_LANG_PUSH(C)
AC_CHECK_HEADER([stdio.h])
AC_CHECK_TYPES([struct __sFILE])	dnl BSD, Darwin
AC_CHECK_TYPES([struct _IO_FILE])	dnl linux
AC_CHECK_TYPES([__FILE_TAG])		dnl Sun (C++)
AC_CHECK_TYPES([struct __FILE_TAG])	dnl Sun (C)
AC_LANG_POP(C)
])

