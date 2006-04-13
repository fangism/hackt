dnl "config/cc.m4"
dnl	$Id: cc.m4,v 1.3 2006/04/13 21:44:39 fang Exp $
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

dnl
dnl NOTE: this macro checks and overwrites the value of 
dnl ac_cv_fun_fmod, which is first defined by AC_CHECK_FUNC.
dnl Recipe from the autoconf-2.59 manua1, section 5.5.4.
dnl
AC_DEFUN([AC_FUNC_FMOD],
[AC_REQUIRE([AC_PROG_CC])
dnl AC_CACHE_CHECK([for fmod], [ac_cv_func_fmod], [
AC_CHECK_FUNCS([fmod])
if test "$ac_cv_func_fmod" = no ; then
	dnl check other candidate libraries
	for lib in m ; do
		AC_CHECK_LIB($lib, fmod,
			[AC_DEFINE(HAVE_FMOD)
			LIBS="$LIBS -l$lib" ; break]
		)
	done
fi
dnl ])
])

dnl
dnl Same idea as above, but checking for fmodf.
dnl
AC_DEFUN([AC_FUNC_FMODF],
[AC_REQUIRE([AC_PROG_CC])
dnl AC_CACHE_CHECK([for fmodf], [ac_cv_func_fmodf], [
AC_CHECK_FUNCS([fmodf])
if test "$ac_cv_func_fmodf" = no ; then
	dnl check other candidate libraries
	for lib in m ; do
		AC_CHECK_LIB($lib, fmodf,
			[AC_DEFINE(HAVE_FMODF)
			LIBS="$LIBS -l$lib" ; break]
		)
	done
fi
dnl ])
])

dnl
dnl Same idea as above, but checking for fmodf.
dnl TODO: generalize this macro for libm
dnl
AC_DEFUN([AC_FUNC_SQRT],
[AC_REQUIRE([AC_PROG_CC])
dnl AC_CACHE_CHECK([for sqrt], [ac_cv_func_sqrt], [
AC_CHECK_FUNCS([sqrt])
if test "$ac_cv_func_sqrt" = no ; then
	dnl check other candidate libraries
	for lib in m ; do
		AC_CHECK_LIB($lib, sqrt,
			[AC_DEFINE(HAVE_SQRT)
			LIBS="$LIBS -l$lib" ; break]
		)
	done
fi
dnl ])
])

dnl
dnl Can we do integer arithmetic operations on quad_t?
dnl on some systems, it is a struct
dnl
AC_DEFUN([AC_TYPE_QUAD_T_ARITHMETIC],
[AC_REQUIRE([AC_PROG_CC])
AC_CHECK_SIZEOF(quad_t)
if test "$ac_cv_sizeof_quad_t" != 0 ; then
AC_CACHE_CHECK([whether quad_t supports arithmetic operations],
	[ac_cv_type_quad_t_is_arithmetic],
	[AC_COMPILE_IFELSE(
		AC_LANG_PROGRAM([
			#if defined(HAVE_INTTYPES_H)
			#include <inttypes.h>
			#endif
			#if defined(HAVE_SYS_TYPES_H)
			#include <sys/types.h>
			#endif
		],[[
			quad_t x = 0, y = 1, z = 2;
			quad_t a = x +y +z;
			quad_t b = y << 1;
			quad_t c = z | y;
			quad_t d = b & c;
			quad_t e = (a ^ b) >> d;
			while (e) {
				++x;
				--e;
			}
		]]),
		[ac_cv_type_quad_t_is_arithmetic=yes],
		[ac_cv_type_quad_t_is_arithmetic=no]
	)]
)
if test "$ac_cv_type_quad_t_is_arithmetic" = yes ; then
	AC_DEFINE(QUAD_T_IS_ARITHMETIC, 1,
		[Define to 1 if quad_t is type and arithmetic operable])
else
	AC_DEFINE(QUAD_T_IS_ARITHMETIC, 0,
		[Define to 1 if quad_t is type and arithmetic operable])
fi
fi
])


dnl
dnl Can we do integer arithmetic operations on quad_t?
dnl on some systems, it is a struct
dnl
AC_DEFUN([AC_TYPE_U_QUAD_T_ARITHMETIC],
[AC_REQUIRE([AC_PROG_CC])
AC_CHECK_SIZEOF(u_quad_t)
if test "$ac_cv_sizeof_u_quad_t" != 0 ; then
AC_CACHE_CHECK([whether u_quad_t supports arithmetic operations],
	[ac_cv_type_u_quad_t_is_arithmetic],
	[AC_COMPILE_IFELSE(
		AC_LANG_PROGRAM([
			#if defined(HAVE_INTTYPES_H)
			#include <inttypes.h>
			#endif
			#if defined(HAVE_SYS_TYPES_H)
			#include <sys/types.h>
			#endif
		],[[
			u_quad_t x = 0, y = 1, z = 2;
			u_quad_t a = x +y +z;
			u_quad_t b = y << 1;
			u_quad_t c = z | y;
			u_quad_t d = b & c;
			u_quad_t e = (a ^ b) >> d;
			while (e) {
				++x;
				--e;
			}
		]]),
		[ac_cv_type_u_quad_t_is_arithmetic=yes],
		[ac_cv_type_u_quad_t_is_arithmetic=no]
	)]
)
if test "$ac_cv_type_u_quad_t_is_arithmetic" = yes ; then
	AC_DEFINE(U_QUAD_T_IS_ARITHMETIC, 1,
		[Define to 1 if u_quad_t is type and arithmetic operable])
else
	AC_DEFINE(U_QUAD_T_IS_ARITHMETIC, 0,
		[Define to 1 if u_quad_t is type and arithmetic operable])
fi
fi
])


