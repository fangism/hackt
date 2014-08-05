dnl "config/cxx_STL.m4"
dnl	$Id: cxx_STL.m4,v 1.18 2011/04/01 01:20:34 fang Exp $
dnl Autoconf macros for detecting variations in C++ STL for any given compiler.
dnl


dnl @synopsis FANG_CXX_STL_REVERSE_ITERATOR_COMPARISONS
dnl
dnl source taken from "src/util/STL/reverse_iterator.hh"
dnl known to be present in gcc-4.1 headers, 
dnl but missing in all previous versions of gcc.  
dnl Defines HAVE_STL_REVERSE_ITERATOR_COMPARISONS if present.  
dnl Updated to account for versioned namespace checking in libc++.
dnl
dnl @category Cxx
dnl @version 2014-07-11
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_STL_REVERSE_ITERATOR_COMPARISONS],
[AC_REQUIRE([AC_PROG_CXX])
AC_REQUIRE([FANG_CXXLIB_VERSIONED_NAMESPACE])
AC_CACHE_CHECK(
	[whether libstdc++ (STL) already contains reverse_iterator comparisons],
[fang_cv_cxx_stl_reverse_iterator_comparisons],
[AC_LANG_PUSH(C++)
saved_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$saved_CPPFLAGS -I$srcdir/src"
AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([[
		#include "util/STL/libconfig.hh"
		#include <iterator>
		BEGIN_NAMESPACE_STD
		template <class Iter1, class Iter2>
		inline bool
		operator == (const reverse_iterator<Iter1>& x,
				const reverse_iterator<Iter2>& y) {
			return x.base() == y.base();
		}
		END_NAMESPACE_STD
		]], []
	),
	[fang_cv_cxx_stl_reverse_iterator_comparisons=no],
	[fang_cv_cxx_stl_reverse_iterator_comparisons=yes]
)
CPPFLAGS=$saved_CPPFLAGS
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_stl_reverse_iterator_comparisons" = "yes" ; then
AC_DEFINE(HAVE_STL_REVERSE_ITERATOR_COMPARISONS, [],
	[True if STL <iterator> header defines reverse_iterator comparisons])
fi
])dnl

dnl @synopsis FANG_CXX_STD_BEGIN_END
dnl
dnl Detect whether std::begin and std::end are provided by C++ library 
dnl Defines HAVE_STD_BEGIN_END if present.  
dnl
dnl @category Cxx
dnl @version 2014-07-14
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_STD_BEGIN_END],
[AC_REQUIRE([AC_PROG_CXX])
AC_CACHE_CHECK(
	[whether std::begin(),std::end() are provided by <iterator>],
[fang_cv_cxx_std_begin_end],
[AC_LANG_PUSH(C++)
dnl saved_CPPFLAGS=$CPPFLAGS
dnl CPPFLAGS="$saved_CPPFLAGS -I$srcdir/src"
AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([[
		#include <vector>
		#include <list>
		#include <iterator>
		#include <numeric>
		]], [
			std::vector<int> a(9);
			std::list<float> b(9);
			return std::accumulate(begin(a), end(a), 1)
				+std::accumulate(begin(b), end(b), 1);
		]
	),
	[fang_cv_cxx_std_begin_end=yes],
	[fang_cv_cxx_std_begin_end=no]
)
dnl CPPFLAGS=$saved_CPPFLAGS
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_std_begin_end" = "yes" ; then
AC_DEFINE(HAVE_STD_BEGIN_END, [],
	[True if <iterator> provides std::begin() and std::end()])
fi
])dnl

dnl @synopsis FANG_CXX_STL_VALARRAY_BEGIN_END
dnl
dnl Detect whether or not std::valarray has non-member begin/end functions.
dnl Defines HAVE_STL_VALARRAY_BEGIN_END if present.  
dnl
dnl @category Cxx
dnl @version 2014-07-14
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_STL_VALARRAY_BEGIN_END],
[AC_REQUIRE([AC_PROG_CXX])
AC_CACHE_CHECK(
	[whether std::valarray supports non-member begin/end functions],
[fang_cv_cxx_stl_valarray_begin_end],
[AC_LANG_PUSH(C++)
dnl saved_CPPFLAGS=$CPPFLAGS
dnl CPPFLAGS="$saved_CPPFLAGS -I$srcdir/src"
AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([[
		#include <valarray>
		#include <numeric>
		]], [
			std::valarray<int> a(99);
			return std::accumulate(begin(a), end(a), 1);
		]
	),
	[fang_cv_cxx_stl_valarray_begin_end=yes],
	[fang_cv_cxx_stl_valarray_begin_end=no]
)
dnl CPPFLAGS=$saved_CPPFLAGS
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_stl_valarray_begin_end" = "yes" ; then
AC_DEFINE(HAVE_STL_VALARRAY_BEGIN_END, [],
	[True if <valarray> provides non-member begin() and end()])
fi
])dnl

dnl @synopsis FANG_CXX_STL_CONSTRUCT
dnl
dnl Check for certain declarations of libstdc++ internal function
dnl std::_Construct, which may have changed.
dnl
dnl 2011-03-31: just use <memory>, test calling std::_Construct
dnl	instead of re-defining it
dnl 2011-02-08: requires <iterator> header for iterator_traits
dnl
dnl @category Cxx
dnl @version 2011-02-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_STL_CONSTRUCT],
[AC_REQUIRE([AC_PROG_CXX])
AC_CACHE_CHECK(
	[whether libstdc++ (STL) contains std::_Construct(T*) default ctor],
[fang_cv_cxx_stl_construct_default],
[AC_LANG_PUSH(C++)
AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([[
		#include <memory>
		struct foo { int bar; foo() : bar(0) { } };
		]], [
		foo x;
		std::_Construct(&x);
		]
	),
	[fang_cv_cxx_stl_construct_default=yes],
	[fang_cv_cxx_stl_construct_default=no]
)
])
if test "$fang_cv_cxx_stl_construct_default" = "yes" ; then
AC_DEFINE(HAVE_STL_CONSTRUCT_DEFAULT, [],
	[True if STL header defines std::_Construct(T*) default ctor])
fi
])dnl
	

dnl @synopsis FANG_CXX_STL_CONSTRUCT_COPY
dnl
dnl Check for certain declarations of libstdc++ internal function
dnl std::_Construct, which may have changed.
dnl
dnl 2011-03-31: just use <memory>, test calling std::_Construct
dnl	instead of re-defining it
dnl 2011-02-08: requires <iterator> header for iterator_traits
dnl
dnl @category Cxx
dnl @version 2011-02-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_STL_CONSTRUCT_COPY],
[AC_REQUIRE([AC_PROG_CXX])
AC_CACHE_CHECK(
	[whether libstdc++ (STL) contains std::_Construct(T*, const T&) copy-ctor],
[fang_cv_cxx_stl_construct_copy],
[AC_LANG_PUSH(C++)
AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([[
		#include <memory>
		struct foo { int bar; foo() : bar(0) { } };
		]], [[
		foo x, y;
		std::_Construct(&x, y);
		]]
	),
	[fang_cv_cxx_stl_construct_copy=yes],
	[fang_cv_cxx_stl_construct_copy=no]
)
])
if test "$fang_cv_cxx_stl_construct_copy" = "yes" ; then
AC_DEFINE(HAVE_STL_CONSTRUCT_COPY, [],
	[True if STL header defines std::_Construct(T*, const T&) copy-ctor])
fi
])dnl
	

dnl @synopsis FANG_CXX_STD_IFSTREAM_DEV_STDIN
dnl
dnl Checks to see if std::ifstream can open "/dev/stdin"
dnl Defines HAVE_STD_IFSTREAM_DEV_STDIN if successful.  
dnl This relies on AC_DEFINES from FANG_STD_FILE_DESCRIPTORS.
dnl
dnl @category Cxx
dnl @version 2007-02-27
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_STD_IFSTREAM_DEV_STDIN],
[AC_REQUIRE([AC_PROG_CXX])
AC_REQUIRE([FANG_STD_FILE_DESCRIPTORS])
AC_CACHE_CHECK(
	[whether std::ifstream works with /dev/stdin],
	[fang_cv_cxx_std_ifstream_dev_stdin],
[AC_LANG_PUSH(C++)
fang_cv_cxx_std_ifstream_dev_stdin=no
AC_LINK_IFELSE(
	AC_LANG_PROGRAM([[
		#include <iostream>
		#include <fstream>
		#include <string>
		using namespace std;
	]], [[
		ifstream fcin(DEV_STDIN);
		if (fcin) {
			string line;
			while (getline(fcin, line)) {
				cout << line << endl;
			}
		} else {
			cout << "You lose." << endl;
		}
	]]),
	[
	if test "$cross_compiling" = yes ; then
		fang_cv_cxx_std_ifstream_dev_stdin=yes
		AC_MSG_NOTICE([Assuming yes because cross-compiling...])
	else
	echo "Hello, world!" > conftest.in
	echo "That's all, folks!" >> conftest.in
	./conftest$ac_exeext < conftest.in > conftest.out 2>&1
	if diff conftest.in conftest.out > /dev/null 2>&1
	then
		fang_cv_cxx_std_ifstream_dev_stdin=yes
	fi
	rm -f conftest.in conftest.out
	fi
	],
	[]
)
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_std_ifstream_dev_stdin" = yes ; then
AC_DEFINE(HAVE_STD_IFSTREAM_DEV_STDIN, [], 
	[Define if std::ifstream(DEV_STDIN) works])
fi
])dnl

dnl @synopsis FANG_CXX_STD_OFSTREAM_DEV_STDOUT
dnl
dnl Checks to see if std::ofstream can open "/dev/stdout"
dnl Defines HAVE_STD_OFSTREAM_DEV_STDOUT if successful.  
dnl
dnl @category Cxx
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_STD_OFSTREAM_DEV_STDOUT],
[AC_REQUIRE([AC_PROG_CXX])
AC_REQUIRE([FANG_STD_FILE_DESCRIPTORS])
AC_CACHE_CHECK(
	[whether std::ofstream works with /dev/stdout],
	[fang_cv_cxx_std_ofstream_dev_stdout],
[AC_LANG_PUSH(C++)
fang_cv_cxx_std_ofstream_dev_stdout=no
AC_LINK_IFELSE(
	AC_LANG_PROGRAM([[
		#include <iostream>
		#include <fstream>
		#include <string>
		using namespace std;
	]], [[
		ofstream fcout(DEV_STDOUT);
		if (cin) {
			string line;
			while (getline(cin, line)) {
				fcout << line << endl;
			}
		} else {
			cout << "You lose." << endl;
		}
	]]),
	[
	if test "$cross_compiling" = yes ; then
		fang_cv_cxx_std_ofstream_dev_stdout=yes
		AC_MSG_NOTICE([Assuming yes because cross-compiling...])
	else
	echo "Hello, world!" > conftest.in
	echo "That's all, folks!" >> conftest.in
	./conftest$ac_exeext < conftest.in > conftest.out 2>&1
	if diff conftest.in conftest.out > /dev/null 2>&1
	then
		fang_cv_cxx_std_ofstream_dev_stdout=yes
	fi
	rm -f conftest.in conftest.out
	fi
	],
	[]
)
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_std_ofstream_dev_stdout" = yes ; then
AC_DEFINE(HAVE_STD_OFSTREAM_DEV_STDOUT, [], 
	[Define if std::ofstream(DEV_STDOUT) works])
fi
])dnl

dnl @synopsis FANG_CXX_STD_OFSTREAM_DEV_STDERR
dnl
dnl Checks to see if std::ofstream can open "/dev/stderr"
dnl Defines HAVE_STD_OFSTREAM_DEV_STDERR if successful.  
dnl
dnl @category Cxx
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_STD_OFSTREAM_DEV_STDERR],
[AC_REQUIRE([AC_PROG_CXX])
AC_REQUIRE([FANG_STD_FILE_DESCRIPTORS])
AC_CACHE_CHECK(
	[whether std::ofstream works with /dev/stderr],
	[fang_cv_cxx_std_ofstream_dev_stderr],
[AC_LANG_PUSH(C++)
fang_cv_cxx_std_ofstream_dev_stderr=no
AC_LINK_IFELSE(
	AC_LANG_PROGRAM([[
		#include <iostream>
		#include <fstream>
		#include <string>
		using namespace std;
	]], [[
		ofstream fcerr(DEV_STDERR);
		if (cin) {
			string line;
			while (getline(cin, line)) {
				fcerr << line << endl;
			}
		} else {
			cerr << "You lose." << endl;
		}
	]]),
	[
	if test "$cross_compiling" = yes ; then
		fang_cv_cxx_std_ofstream_dev_stderr=yes
		AC_MSG_NOTICE([Assuming yes because cross-compiling...])
	else
	echo "Hello, world!" > conftest.in
	echo "That's all, folks!" >> conftest.in
	./conftest$ac_exeext < conftest.in > conftest.out 2>&1
	if diff conftest.in conftest.out > /dev/null 2>&1
	then
		fang_cv_cxx_std_ofstream_dev_stderr=yes
	fi
	rm -f conftest.in conftest.out
	fi
	],
	[]
)
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_std_ofstream_dev_stderr" = yes ; then
AC_DEFINE(HAVE_STD_OFSTREAM_DEV_STDERR, [], 
	[Define if std::ofstream(DEV_STDERR) works])
fi
])dnl

dnl @synopsis FANG_HEADER_STDCXX
dnl
dnl Check for presence and location of "standard" and "nonstandard" C++ headers.
dnl NOTE: not all these headers are STL, (some are just libstdc++)
dnl but these belong here better than in "cxx.m4" which checks more
dnl for compiler and language attributes.
dnl
dnl @category Cxx
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_HEADER_STDCXX],
[AC_REQUIRE([AC_PROG_CXX])
AC_LANG_PUSH(C++)
dnl C++ wrappers to standard C headers
AC_CHECK_HEADERS([cstddef cstdlib cstdio cstring cassert cmath cctype \]
	[cerrno cfloat climits clocale csignal csetjmp cstdarg ctime \]
	[cwchar cwctype \]
dnl checking all possible locations of (some equivalent) header files
dnl spanning versions gcc-2.95 to 4.x
dnl I bet you didn't know some of these locations were ever used!
dnl However, you're not supposed to use anything older than gcc-3.3 anyways...
dnl where is that damn hash_map?
	[ext/hash_map hash_map alt/hash_map \]
	[ext/hashtable.h \]
dnl used mainly in "src/util/hash_specializations.hh"
	[ext/stl_hash_fun.h ext/hash_fun.h \]
	[stl_hash_fun.h bits/functional_hash.h\]
	[ext/hash_set hash_set alt/hash_set \]
	[ext/slist slist alt/slist \]
	[ext/hashtable.h tr1/hashtable hashtable.h \]
	[unordered_map tr1/unordered_map unordered_set tr1/unordered_set \]
	[ext/new_allocator.h \]
	[ext/functional tr1/functional \]
dnl used in "src/util/string_fwd.hh"
	[bits/stringfwd.h \]
	[bits/concept_check.h \]
dnl bits/stl_contruct.h requires <iterator> for iterator traits
	[bits/stl_construct.h \]
	[type_traits bits/type_traits.h \]
	[tr1/type_traits tr1/type_traits_fwd.h type_traits.h \]
dnl among these, only sstream should be used, the rest are long gone
	[sstream strstream stringstream \]
dnl can't include <tr1/boost_shared_ptr.h> directly
	[tr1/memory random tr1/random regex tr1/random \]
	[tuple tr1/tuple array tr1/array])
AC_LANG_POP(C++)
])dnl

dnl @synopsis FANG_HASH_MAP_NAMESPACE
dnl
dnl Detect the home namespace of hash_map.
dnl The location/existence of the header file does NOT necessarily correlate
dnl with the home namespace!  (Lesson learned)
dnl Defines HASH_MAP_IN_STD or HASH_MAP_IN___GNU_CXX.
dnl
dnl @category Cxx
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_HASH_MAP_NAMESPACE],
[AC_REQUIRE([FANG_HEADER_STDCXX])
cat > conftest2.h <<ACEOF
	#if	defined(HAVE_EXT_HASH_MAP)
	#include <ext/hash_map>
	#elif	defined(HAVE_HASH_MAP)
	#include <hash_map>
	#elif	defined(HAVE_ALT_HASH_MAP)
	#include <alt/hash_map>
	#else
	#error	"Hash_map, what hash_map?"
	#endif
ACEOF

AC_CACHE_CHECK([if hash_map resides in the std namespace],
[fang_cv_hash_map_std_namespace],
[AC_LANG_PUSH(C++)
AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([[
		#include "conftest2.h"
		using std::hash_map;
		typedef	hash_map<int, int>	map_type;
	]], [[
		map_type m;
		m.clear();
	]]),
	[fang_cv_hash_map_std_namespace=yes],
	[fang_cv_hash_map_std_namespace=no]
)
AC_LANG_POP(C++)
])
if test "$fang_cv_hash_map_std_namespace" = yes ; then
	AC_DEFINE(HASH_MAP_IN_STD, [],
		[Define if std::hash_map exists])
fi

AC_CACHE_CHECK([if hash_map resides in the __gnu_cxx namespace],
[fang_cv_hash_map___gnu_cxx_namespace],
[AC_LANG_PUSH(C++)
AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([[
		#include "conftest2.h"
		using __gnu_cxx::hash_map;
		typedef	hash_map<int, int>	map_type;
	]], [[
		map_type m;
		m.clear();
	]]),
	[fang_cv_hash_map___gnu_cxx_namespace=yes],
	[fang_cv_hash_map___gnu_cxx_namespace=no]
)
AC_LANG_POP(C++)
])
if test "$fang_cv_hash_map___gnu_cxx_namespace" = yes ; then
	AC_DEFINE(HASH_MAP_IN___GNU_CXX, [],
		[Define if __gnu_cxx::hash_map exists])
fi

rm -f conftest2.h
])dnl


dnl @synopsis HAVE_HASH_MAP_DEPRECATED
dnl
dnl Checks whether or not any hash_map is deprecated, especially for newer
dnl compiler in strict ANSI/ISO mode.
dnl
dnl @category Cxx
dnl @version 2008-11-24
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_HASH_MAP_DEPRECATED],
[AC_REQUIRE([FANG_HASH_MAP_NAMESPACE])
AC_CACHE_CHECK([if hash_map is deprecated],
[fang_cv_hash_map_deprecated],
[AC_LANG_PUSH(C++)
dnl we want deprecation warnings to be promoted errors for this test
saved_CXXFLAGS=$CXXFLAGS
CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([[
	#ifdef HAVE_EXT_HASH_MAP
	#include <ext/hash_map>
	#elif defined(HAVE_HASH_MAP)
	#include <hash_map>
	#endif
	#ifdef	HASH_MAP_IN___GNU_CXX
	#define	HASH_MAP_NS	__gnu_cxx
	#else
	#define	HASH_MAP_NS	std
	#endif
	using HASH_MAP_NS::hash_map;
	typedef	hash_map<int, int> map_type;
	static map_type foo;
	]], []),
	[fang_cv_hash_map_deprecated=no],
	[fang_cv_hash_map_deprecated=yes]
)
CXXFLAGS="$saved_CXXFLAGS"
AC_LANG_POP
])
if test "$fang_cv_hash_map_deprecated" = "yes" ; then
	AC_DEFINE(HASH_MAP_DEPRECATED, [], [Define if hash_maps are deprecated])
fi
])

dnl @synopsis FANG_HASH_MAP_STYLE
dnl
dnl Checks whether or not the hash_map has 4 or 5 template parameters.
dnl SGI-style uses separate hash and compare functors (5 params).
dnl Intel style uses a composed hash_compare binary functor (4 params).  
dnl Defines HASH_MAP_SGI_STYLE or HASH_MAP_INTEL_STYLE.  
dnl
dnl @category Cxx
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_HASH_MAP_STYLE],
[AC_REQUIRE([FANG_HASH_MAP_NAMESPACE])

cat > conftest2.h <<ACEOF
	#if	defined(HAVE_EXT_HASH_MAP)
	#include <ext/hash_map>
	#elif	defined(HAVE_HASH_MAP)
	#include <hash_map>
	#elif	defined(HAVE_ALT_HASH_MAP)
	#include <alt/hash_map>
	#else
	#error	"Hash_map, what hash_map?"
	#endif
ACEOF

if test "$fang_cv_hash_map_std_namespace" = yes ; then
cat >> conftest2.h <<ACEOF
using std::hash_map;
ACEOF
elif test "$fang_cv_hash_map___gnu_cxx_namespace" = yes ; then
cat >> conftest2.h <<ACEOF
using __gnu_cxx::hash_map;
ACEOF
fi

dnl SGI-style uses separate hash and compare template parameters
AC_CACHE_CHECK([if hash_map is SGI-style],
[fang_cv_hash_map_sgi_style],
[AC_LANG_PUSH(C++)
AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([[
		#include "conftest2.h"
		template <class K>
		struct bogus_hash {
			std::size_t
			operator () (const K&) const { return 0; }
		};
		typedef	hash_map<int, int, bogus_hash<int>, 
			std::less<int>, std::allocator<int> >	map_type;
	]], [[
		map_type m;
		m.clear();
		m.find(0);
	]]),
	[fang_cv_hash_map_sgi_style=yes],
	[fang_cv_hash_map_sgi_style=no]
)
AC_LANG_POP(C++)
])
if test "$fang_cv_hash_map_sgi_style" = yes ; then
	AC_DEFINE(HASH_MAP_SGI_STYLE, [], [Define if hash_map is SGI-style])
fi

AC_CACHE_CHECK([if hash_map is Intel-style],
[fang_cv_hash_map_intel_style],
[AC_LANG_PUSH(C++)
AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([[
		#include "conftest2.h"
		template <class K, class C>
		struct bogus_hash_compare {
			enum { bucket_size = 4 };
			enum { min_buckets = 8 };
			std::size_t
			operator () (const K&) const { return 0; }
			bool
			operator () (const K&, const K&) const { return false; }
		};
		typedef	hash_map<int, int,
			bogus_hash_compare<int, std::less<int> >, 
			std::allocator<int> >			map_type;
	]], [[
		map_type m;
		m.clear();
		m.find(0);
	]]),
	[fang_cv_hash_map_intel_style=yes],
	[fang_cv_hash_map_intel_style=no]
)
AC_LANG_POP(C++)
])
if test "$fang_cv_hash_map_intel_style" = yes ; then
	AC_DEFINE(HASH_MAP_INTEL_STYLE, [], [Define if hash_map is Intel-style])
fi
])dnl

dnl @synopsis FANG_CXX_STL_FUNCTIONAL_SELECT
dnl
dnl Three tests combined in one.
dnl Checks if <functional> contains _Identity, _Select1st, _Select2nd.
dnl Defines HAVE_STD__IDENTITY, HAVE_STD__SELECT1ST, HAVE_STD_SELECT__2ND
dnl respectively, if they are available and defined.  
dnl In source, "util/STL/functional.hh" provide them if they are missing.  
dnl
dnl @category Cxx
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_STL_FUNCTIONAL_SELECT],
[AC_REQUIRE([FANG_HEADER_STDCXX])

AC_CACHE_CHECK([whether <functional> contains std::_Identity],
[fang_cv_cxx_stl_functional_identity],
[AC_LANG_PUSH(C++)
AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([
		#include <functional>
	], [
		int i = 4;
		int j = std::_Identity<int>()(i);
		while (j) --j;
	]),
	[fang_cv_cxx_stl_functional_identity=yes],
	[fang_cv_cxx_stl_functional_identity=no]
)
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_stl_functional_identity" = yes ; then
	AC_DEFINE(HAVE_STD__IDENTITY, [],
		[Define if STL contains _Identity functor])
fi

AC_CACHE_CHECK([whether <functional> contains std::_Select1st],
[fang_cv_cxx_stl_functional_select1st],
[AC_LANG_PUSH(C++)
AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([
		#include <utility>
		#include <functional>
	], [[
		typedef	std::pair<int, int>	pair_type;
		pair_type i(4,8);
		int j = std::_Select1st<pair_type>()(i);
		while (j) --j;
	]]),
	[fang_cv_cxx_stl_functional_select1st=yes],
	[fang_cv_cxx_stl_functional_select1st=no]
)
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_stl_functional_select1st" = yes ; then
	AC_DEFINE(HAVE_STD__SELECT1ST, [],
		[Define if STL contains _Select1st functor])
fi

AC_CACHE_CHECK([whether <functional> contains std::_Select2nd],
[fang_cv_cxx_stl_functional_select2nd],
[AC_LANG_PUSH(C++)
AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([
		#include <utility>
		#include <functional>
	], [[
		typedef	std::pair<int, int>	pair_type;
		pair_type i(4,8);
		int j = std::_Select2nd<pair_type>()(i);
		while (j) --j;
	]]),
	[fang_cv_cxx_stl_functional_select2nd=yes],
	[fang_cv_cxx_stl_functional_select2nd=no]
)
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_stl_functional_select2nd" = yes ; then
	AC_DEFINE(HAVE_STD__SELECT2ND, [],
		[Define if STL contains _Select2nd functor])
fi

])dnl


dnl @synopsis FANG_CXX_STL_BITSET_EXTENSIONS
dnl
dnl Two tests combined in one.
dnl Checks if <bitset> has member functions _Find_first(), _Find_next(),
dnl as given by GLIBC++'s SGI extensions.  
dnl Defines HAVE_STD_BITSET_FIND_FIRST, HAVE_STD_BITSET_FIND_NEXT
dnl respectively, if they are available and defined.  
dnl In source, "util/bitset.hh" provides a common interface in either case.
dnl
dnl @category Cxx
dnl @version 2007-02-20
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_STL_BITSET_EXTENSIONS],
[AC_REQUIRE([FANG_HEADER_STDCXX])

AC_CACHE_CHECK([whether std::bitset contains _Find_first()],
[fang_cv_cxx_stl_bitset_find_first],
[AC_LANG_PUSH(C++)
AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([
		#include <bitset>
	], [
		std::bitset<24> foo(0xbad0);
		return (foo._Find_first() != 4);
	]),
	[fang_cv_cxx_stl_bitset_find_first=yes],
	[fang_cv_cxx_stl_bitset_find_first=no]
)
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_stl_bitset_find_first" = yes ; then
	AC_DEFINE(HAVE_STD_BITSET_FIND_FIRST, [],
		[Define if std::bitset has _Find_first() method])
	dnl added this test because at least one system fails
	AC_CACHE_CHECK([whether std::bitset::_Find_first is defined in libstdc++],
		[fang_cv_cxx_stl_bitset_find_first_in_lib],
	[AC_LANG_PUSH(C++)
		AC_LINK_IFELSE(
			AC_LANG_PROGRAM([
				#include <bitset>
			], [
				std::bitset<24> foo(0xbad0);
				return (foo._Find_first() != 4);
			]),
			[fang_cv_cxx_stl_bitset_find_first_in_lib=yes],
			[fang_cv_cxx_stl_bitset_find_first_in_lib=no]
		)
	AC_LANG_POP(C++)
	])
	if test "$fang_cv_cxx_stl_bitset_find_first_in_lib" = yes ; then
		AC_DEFINE(HAVE_STD_BITSET_FIND_FIRST_IN_LIB, [],
			[Define if std::bitset::_Find_first() is in libstdc++])
	else
		AC_MSG_NOTICE([Lovely, your libstdc++ is horked...])
	fi
fi

AC_CACHE_CHECK([whether std::bitset contains _Find_next()],
[fang_cv_cxx_stl_bitset_find_next],
[AC_LANG_PUSH(C++)
AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([
		#include <bitset>
	], [
		std::bitset<24> foo(0xf00d);
		return (foo._Find_next(7) != 12);
	]),
	[fang_cv_cxx_stl_bitset_find_next=yes],
	[fang_cv_cxx_stl_bitset_find_next=no]
)
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_stl_bitset_find_next" = yes ; then
	AC_DEFINE(HAVE_STD_BITSET_FIND_NEXT, [],
		[Define if std::bitset has _Find_next() method])
	dnl added this test because at least one system fails
	AC_CACHE_CHECK([whether std::bitset::_Find_next is defined in libstdc++],
		[fang_cv_cxx_stl_bitset_find_next_in_lib],
	[AC_LANG_PUSH(C++)
		AC_LINK_IFELSE(
		AC_LANG_PROGRAM([
			#include <bitset>
		], [
			std::bitset<24> foo(0xf00d);
			return (foo._Find_next(7) != 12);
		]),
			[fang_cv_cxx_stl_bitset_find_next_in_lib=yes],
			[fang_cv_cxx_stl_bitset_find_next_in_lib=no]
		)
	AC_LANG_POP(C++)
	])
	if test "$fang_cv_cxx_stl_bitset_find_next_in_lib" = yes ; then
		AC_DEFINE(HAVE_STD_BITSET_FIND_NEXT_IN_LIB, [],
			[Define if std::bitset::_Find_next() is in libstdc++])
	else
		AC_MSG_NOTICE([Wonderful, your libstdc++ is horked...])
	fi
fi
])dnl

dnl @synopsis FANG_CXX_STL_TREE
dnl Checks whether or not std::set/map are based upon a red-black tree
dnl with implementation named _Rb_tree*
dnl
AC_DEFUN([FANG_CXX_STL_TREE],
[AC_REQUIRE([AC_PROG_CXX])
AC_CACHE_CHECK(
	[whether std::set,map are based on _Rb_tree], 
[fang_cv_cxx_stl_tree],
[AC_LANG_PUSH(C++)
dnl saved_CXXFLAGS=$CXXFLAGS
dnl CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([[
		#include <set>
		#include <map>
		using std::_Rb_tree_node_base;
		using std::_Rb_tree_node;
		using std::_Rb_tree;
		]], []),
	[fang_cv_cxx_stl_tree=yes],
	[fang_cv_cxx_stl_tree=no]
)
dnl CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_stl_tree" = "yes" ; then
AC_DEFINE(HAVE_STL_TREE, [],
	[True if STL <set> is based on std::_Rb_tree])
fi
])

dnl @synopsis FANG_CXX_STL_COPY_IF
dnl
dnl Defines HAVE_STL_COPY_IF if present.  
dnl known to be present in gcc-4.4 c++0x mode.
dnl
dnl @category Cxx
dnl @version 2009-04-25
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_STL_COPY_IF],
[AC_REQUIRE([AC_PROG_CXX])
AC_REQUIRE([FANG_CXXLIB_VERSIONED_NAMESPACE])
AC_CACHE_CHECK(
	[whether libstdc++ (STL) already contains copy_if algorithm],
[fang_cv_cxx_stl_copy_if],
[AC_LANG_PUSH(C++)
saved_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$saved_CPPFLAGS -I$srcdir/src"
AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([[
		#include "util/STL/libconfig.hh"
		#include <algorithm>
		BEGIN_NAMESPACE_STD
		template <class In, class Out, class Pred>
		Out
		copy_if(In first, In last, Out res, Pred p) {
			while (first != last) {
				if (p(*first)) {
					*res++ = *first;
				}
				++first;
			}
			return res;
		}
		END_NAMESPACE_STD
		]], []
	),
	[fang_cv_cxx_stl_copy_if=no],
	[fang_cv_cxx_stl_copy_if=yes]
)
CPPFLAGS=$saved_CPPFLAGS
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_stl_copy_if" = "yes" ; then
AC_DEFINE(HAVE_STL_COPY_IF, [],
	[True if STL <algorithm> header defines copy_if algorithm])
fi
])dnl


dnl @synopsis FANG_CXX_ISTREAM_NEGATIVE_UNSIGNED
dnl
dnl Checks to see if parsing -1 to an unsigned type sets the 
dnl istream's fail() bit.  
dnl This detects a bug in older libstdc++:
dnl	http://gcc.gnu.org/bugzilla/show_bug.cgi?id=39802
dnl
dnl Defines CXX_ISTREAM_NEGATIVE_UNSIGNED_FAILS if successful.  
dnl
dnl Test is known to fail for i386-gcc-3.4.x, but works for x86_64-gcc-3.4.x.
dnl
dnl @category Cxx
dnl @version 2011-03-30
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_ISTREAM_NEGATIVE_UNSIGNED],
[AC_REQUIRE([AC_PROG_CXX])
AC_CACHE_CHECK(
	[whether std::istream >> (unsigned) fails on negative numbers],
	[fang_cv_cxx_std_istream_negative_unsigned_fail],
[AC_LANG_PUSH(C++)
dnl default: assume standard-confirming behavior when cross-compiling
AC_RUN_IFELSE(
	AC_LANG_PROGRAM([[
		#include <iostream>
		#include <sstream>
		using namespace std;
	]], [[
		istringstream iss("-1");
		unsigned int ui;
		iss >> ui;
		return !iss.fail();
	]]),
	[fang_cv_cxx_std_istream_negative_unsigned_fail=yes],
	[fang_cv_cxx_std_istream_negative_unsigned_fail=no],
	[fang_cv_cxx_std_istream_negative_unsigned_fail=yes]
)
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_std_istream_negative_unsigned_fail" = yes ; then
AC_DEFINE(CXX_ISTREAM_NEGATIVE_UNSIGNED_FAILS, [],
	[Define if std::istream properly fails with reading -1 to an unsigned])
fi
])dnl

dnl @synopsis FANG_CXXLIB_VERSIONED_NAMESPACE
dnl
dnl Checks to see whether or not C++ library is in versioned namespace.
dnl At the time of writing GNU libstdc++ is not in a versioned namespace
dnl while libc++ is, like std::__1.
dnl
dnl Defines CXXLIB_VERSIONED_NAMESPACE if successful.  
dnl This is used in src/util/STL/libconfig.hh.
dnl You may include that and use BEGIN/END_NAMESPACE_STD in other
dnl C++ standard library tests in this file.
dnl
dnl @category Cxx
dnl @version 2014-04-22
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXXLIB_VERSIONED_NAMESPACE],
[AC_REQUIRE([AC_PROG_CXX])
AC_CACHE_CHECK(
	[C++ standard library lives in a versioned namespace],
	[fang_cv_cxx_stdlib_versioned_namespace],
[AC_LANG_PUSH(C++)
dnl default: assume standard-confirming behavior when cross-compiling
AC_RUN_IFELSE(
	AC_LANG_PROGRAM([[
		#include <utility>
		using std::pair;
	]], [[
#if defined(_LIBCPP_ABI_VERSION) && defined(_LIBCPP_NAMESPACE)
		return 0;
#else
		return 1;
#endif
	]]),
	[fang_cv_cxx_stdlib_versioned_namespace=yes],
	[fang_cv_cxx_stdlib_versioned_namespace=no],
	[fang_cv_cxx_stdlib_versioned_namespace=no]
)
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_stdlib_versioned_namespace" = yes ; then
AC_DEFINE(CXXLIB_VERSIONED_NAMESPACE, [],
	[Define if C++ library lives in a versioned namespace])
fi
AM_CONDITIONAL(HAVE_LIBCXX, test "$fang_cv_cxx_stdlib_versioned_namespace" = yes)
])dnl

