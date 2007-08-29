dnl "config/cc.m4"
dnl	$Id: cc.m4,v 1.6 2007/08/29 19:50:11 fang Exp $
dnl General configure macros for detecting characteristics of the C compiler.
dnl

dnl @synopsis FANG_TYPEDEF_FILE
dnl
dnl Checks for common underlying typedefs for the C-stdio FILE type.  
dnl The purpose of this is to be able to forward declare a FILE*
dnl without having to include <stdio.h>
dnl In the event that none of the equivalent typedefs are found, 
dnl the source should just fallback including <stdio.h>.
dnl Source affected: "util/FILE_fwd.h"
dnl
dnl @category C
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_TYPEDEF_FILE],
[AC_REQUIRE([AC_PROG_CC])
AC_LANG_PUSH(C)
AC_CHECK_HEADER([stdio.h])
AC_CHECK_TYPES([struct __sFILE])	dnl BSD, Darwin
AC_CHECK_TYPES([struct _IO_FILE])	dnl linux
AC_CHECK_TYPES([__FILE_TAG])		dnl Sun (C++)
AC_CHECK_TYPES([struct __FILE_TAG])	dnl Sun (C)
AC_LANG_POP(C)
])dnl

dnl @synopsis FANG_C_BUILTIN_EXPECT
dnl
dnl Checks for __builtin_expect compiler directive, useful for 
dnl static branch-prediction hints.  
dnl Recommended for error-handling flow control and other 
dnl unlikely code paths, for example.  
dnl Defines HAVE_BUILTIN_EXPECT if compiler supports __builtin_expect.  
dnl Source affected: "util/likely.h"
dnl
dnl @category C
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_C_BUILTIN_EXPECT],
[AC_REQUIRE([AC_PROG_CC])
AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])	dnl from "config/cxx.m4"
AC_CACHE_CHECK([whether C compiler accepts __builtin_expect()],
[fang_cv_c_builtin_expect],
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
		[fang_cv_c_builtin_expect=yes],
		[fang_cv_c_builtin_expect=no]
	)
	CFLAGS="$saved_CFLAGS"
AC_LANG_POP(C)
])
if test x"$fang_cv_c_builtin_expect" = "xyes" ; then
AC_DEFINE(HAVE_BUILTIN_EXPECT, [], 
	[Define if compiler supports __builtin_expect])
fi
])dnl

dnl @synopsis FANG_FUNC_FMOD
dnl
dnl Checks for fmod function, and also searches libm.  
dnl Defines HAVE_FMOD if fmod is found.
dnl Appends library to LIBS if needed.  
dnl NOTE: this macro checks and overwrites the value of 
dnl ac_cv_func_fmod, which is first defined by AC_CHECK_FUNC.
dnl Recipe from the autoconf-2.59 manua1, section 5.5.4.
dnl
dnl @category C
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_FUNC_FMOD],
[AC_REQUIRE([AC_PROG_CC])
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
])dnl

dnl @synopsis FANG_FUNC_FMODF
dnl
dnl Checks for fmodf function, and also searches libm.  
dnl Defines HAVE_FMODF if fmodf is found.
dnl Appends library to LIBS if needed.  
dnl NOTE: this macro checks and overwrites the value of 
dnl ac_cv_func_fmodf, which is first defined by AC_CHECK_FUNC.
dnl Recipe from the autoconf-2.59 manua1, section 5.5.4.
dnl
dnl @category C
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_FUNC_FMODF],
[AC_REQUIRE([AC_PROG_CC])
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
])dnl

dnl @synopsis FANG_FUNC_SQRT
dnl
dnl Checks for sqrt function, and also searches libm.  
dnl Defines HAVE_SQRT if sqrt is found.
dnl Appends library to LIBS if needed.  
dnl NOTE: this macro checks and overwrites the value of 
dnl ac_cv_func_sqrt, which is first defined by AC_CHECK_FUNC.
dnl Recipe from the autoconf-2.59 manua1, section 5.5.4.
dnl
dnl @category C
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_FUNC_SQRT],
[AC_REQUIRE([AC_PROG_CC])
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
])dnl

dnl @synopsis FANG_TYPE_QUAD_T_ARITHMETIC
dnl
dnl Can we do integer arithmetic operations on quad_t?
dnl on some systems, it is a struct, on others, an integral typedef.  
dnl Defines QUAD_T_IS_ARITHMETIC to 1 if quad_t is integral and arithmetic,
dnl else defines it to 0.  Will remain undefined if quad_t type is not found.
dnl
dnl @category C
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_TYPE_QUAD_T_ARITHMETIC],
[AC_REQUIRE([AC_PROG_CC])
AC_CHECK_SIZEOF(quad_t)
if test "$ac_cv_sizeof_quad_t" != 0 ; then
AC_CACHE_CHECK([whether quad_t supports arithmetic operations],
	[fang_cv_type_quad_t_is_arithmetic],
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
		[fang_cv_type_quad_t_is_arithmetic=yes],
		[fang_cv_type_quad_t_is_arithmetic=no]
	)]
)
if test "$fang_cv_type_quad_t_is_arithmetic" = yes ; then
	AC_DEFINE(QUAD_T_IS_ARITHMETIC, 1,
		[Define to 1 if quad_t is type and arithmetic operable])
else
	AC_DEFINE(QUAD_T_IS_ARITHMETIC, 0,
		[Define to 1 if quad_t is type and arithmetic operable])
fi
fi
])dnl


dnl @synopsis FANG_TYPE_U_QUAD_T_ARITHMETIC
dnl
dnl Can we do integer arithmetic operations on u_quad_t?
dnl on some systems, it is a struct, on others, an integral typedef.  
dnl Defines U_QUAD_T_IS_ARITHMETIC to 1 if u_quad_t is integral and arithmetic,
dnl else defines it to 0.  Will remain undefined if u_quad_t type is not found.
dnl
dnl @category C
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_TYPE_U_QUAD_T_ARITHMETIC],
[AC_REQUIRE([AC_PROG_CC])
AC_CHECK_SIZEOF(u_quad_t)
if test "$ac_cv_sizeof_u_quad_t" != 0 ; then
AC_CACHE_CHECK([whether u_quad_t supports arithmetic operations],
	[fang_cv_type_u_quad_t_is_arithmetic],
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
		[fang_cv_type_u_quad_t_is_arithmetic=yes],
		[fang_cv_type_u_quad_t_is_arithmetic=no]
	)]
)
if test "$fang_cv_type_u_quad_t_is_arithmetic" = yes ; then
	AC_DEFINE(U_QUAD_T_IS_ARITHMETIC, 1,
		[Define to 1 if u_quad_t is type and arithmetic operable])
else
	AC_DEFINE(U_QUAD_T_IS_ARITHMETIC, 0,
		[Define to 1 if u_quad_t is type and arithmetic operable])
fi
fi
])dnl


dnl @synopsis FANG_TYPE_EQUIV_SIZE_T_UNSIGNED_LONG
dnl
dnl Workaround SuSE-linux-g++3.3 defect, where size_t is not considered
dnl the exact same as unsigned long.
dnl AC_DEFINEs TYPE_EQUIV_SIZE_T_UNSIGNED_LONG to 0 or 1.
dnl This result is used in util/numeric/inttype_traits.h.
dnl
AC_DEFUN([FANG_TYPE_EQUIV_SIZE_T_UNSIGNED_LONG],
[AC_CHECK_SIZEOF(size_t)
AC_CHECK_SIZEOF(unsigned long)
test "$ac_cv_sizeof_size_t" != 0 && \
	test "$ac_cv_sizeof_unsigned_long" != 0 || \
	AC_MSG_ERROR([size_t and unsigned long are not both defined types!])
AC_CACHE_CHECK([whether C++ considers size_t and unsigned long the same],
	[fang_cv_type_equiv_size_t_unsigned_long],
	[save_CPPFLAGS=$CPPFLAGS
	CPPFLAGS="$CPPFLAGS -I $srcdir/src"
	AC_COMPILE_IFELSE(
		AC_LANG_PROGRAM([
#include "util/static_assert.h"
#include "util/type_traits.h"
#include "util/size_t.h"
		],[[
        UTIL_STATIC_ASSERT((util::is_same<size_t, size_t>::value));
        UTIL_STATIC_ASSERT((util::is_same<size_t, unsigned long>::value));
		]]),
		[fang_cv_type_equiv_size_t_unsigned_long=yes],
		[fang_cv_type_equiv_size_t_unsigned_long=no]
	)dnl AC_COMPILE_IFELSE

	CPPFLAGS=$save_CPPFLAGS
	]
)dnl AC_CACHE_CHECK

if test "$fang_cv_type_equiv_size_t_unsigned_long" = yes ; then
	AC_DEFINE(TYPE_EQUIV_SIZE_T_UNSIGNED_LONG, 1,
		[Define to 1 if size_t is identical to unsigned long (C++).])
else
	AC_DEFINE(TYPE_EQUIV_SIZE_T_UNSIGNED_LONG, 0,
		[Define to 1 if size_t is identical to unsigned long (C++).])
fi
])dnl AC_DEFUN

