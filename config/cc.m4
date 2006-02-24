dnl "config/cc.m4"
dnl	$Id: cc.m4,v 1.1.2.2 2006/02/24 06:11:43 fang Exp $
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

dnl
dnl Checks for __builtin_expect directive.  
dnl
AC_DEFUN([AC_C_BUILTIN_EXPECT],
[AC_REQUIRE([AC_PROG_CC])
AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])	dnl from "config/cxx.m4"
AC_CACHE_CHECK([whether C compiler accepts __builtin_expect()],
[ac_cv_c_builtin_expect],
[AC_LANG_PUSH(C)
	saved_CFLAGS=$CFLAGS
	CFLAGS="$saved_CFLAGS $ANAL_FLAGS"
	AC_COMPILE_IFELSE(
		AC_LANG_PROGRAM([#include <stdio.h>],[
		int c;
		do {
			c = getchar();
		} while (__builtin_expect(!feof(stdin), 1));
		]),
		[ac_cv_c_builtin_expect=yes],
		[ac_cv_c_builtin_expect=no]
	)
	CFLAGS="$saved_CFLAGS"
AC_LANG_POP(C)
])
if test x"$ac_cv_c_builtin_expect" = "xyes" ; then
AC_DEFINE(HAVE_BUILTIN_EXPECT, [], 
	[Define if compiler supports __builtin_expect])
fi
])

