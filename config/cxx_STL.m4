dnl "config/cxx_STL.m4"
dnl	$Id: cxx_STL.m4,v 1.1.2.1 2006/02/23 04:36:12 fang Exp $
dnl Autoconf macros for detecting variations in C++ STL for any given compiler.
dnl


dnl source taken from "src/util/STL/reverse_iterator.h"
dnl known to be present in gcc-4.1 headers, but missing in all previous versions
AC_DEFUN([AC_CXX_STL_REVERSE_ITERATOR_COMPARISONS],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])	dnl from "config/cxx.m4"
AC_CACHE_CHECK(
	[whether libstdc++ (STL) already contains reverse_iterator comparisons],
[ac_cv_cxx_stl_reverse_iterator_comparisons],
[AC_LANG_PUSH(C++)
saved_CXXFLAGS=$CXXFLAGS
CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([[
		#include <iterator>
		namespace std {
		template <class Iter1, class Iter2>
		inline bool
		operator == (const reverse_iterator<Iter1>& x,
				const reverse_iterator<Iter2>& y) {
			return x.base() == y.base();
		}
		}]], []
	),
	[ac_cv_cxx_stl_reverse_iterator_comparisons=no],
	[ac_cv_cxx_stl_reverse_iterator_comparisons=yes]
)
CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])

if test "$ac_cv_cxx_stl_reverse_iterator_comparisons" = "yes" ; then
AC_DEFINE(HAVE_STL_REVERSE_ITERATOR_COMPARISONS, [],
	[True if STL <iterator> header defines reverse_iterator comparisons])
fi
])


dnl
dnl Check for presence and location of "standard" C++ headers.
dnl NOTE: not all these headers are STL, (some are just libstdc++)
dnl but these belong here bettern than in "cxx.m4" which checks more
dnl for compiler and language attributes.
dnl
AC_DEFUN([AC_HEADER_STDCXX],
[AC_REQUIRE([AC_PROG_CXX])
AC_LANG_PUSH(C++)
dnl C++ wrappers to standard C headers
AC_CHECK_HEADERS([cstddef cstdlib cstdio cstring cassert cmath cctype])
AC_CHECK_HEADERS([cerrno cfloat climits clocale csignal csetjmp cstdarg ctime])
AC_CHECK_HEADERS([cwchar cwctype])
dnl checking all possible locations of (some equivalent) header files
dnl spanning versions gcc-2.95 to 4.x
dnl I bet you didn't know some of these locations were ever used!
dnl However, you're not supposed to use anything older than gcc-3.3 anyways...
dnl where is that damn hash_map?
AC_CHECK_HEADERS([ext/hash_map hash_map])
dnl used mainly in "src/util/hash_specializations.h"
AC_CHECK_HEADERS([ext/stl_hash_fun.h ext/hash_fun.h stl_hash_fun.h])
AC_CHECK_HEADERS([ext/hash_set hash_set])
AC_CHECK_HEADERS([ext/hashtable.h tr1/hashtable hashtable.h])
AC_CHECK_HEADERS([tr1/unordered_map tr1/unordered_set])
AC_CHECK_HEADERS([ext/functional tr1/functional])
dnl used in "src/util/string_fwd.h"
AC_CHECK_HEADERS([bits/stringfwd.h])
AC_CHECK_HEADERS([bits/concept_check.h])
AC_CHECK_HEADERS([bits/type_traits.h tr1/type_traits tr1/type_traits_fwd.h type_traits.h])
dnl among these, only sstream should be used, the rest are long gone
AC_CHECK_HEADERS([sstream strstream stringstream])
dnl can't include <tr1/boost_shared_ptr.h> directly
AC_CHECK_HEADERS([tr1/memory])
AC_CHECK_HEADERS([tr1/tuple tr1/array])
AC_LANG_POP(C++)
])

