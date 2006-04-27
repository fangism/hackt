dnl "config/cxx_STL.m4"
dnl	$Id: cxx_STL.m4,v 1.4 2006/04/27 00:14:35 fang Exp $
dnl Autoconf macros for detecting variations in C++ STL for any given compiler.
dnl


dnl source taken from "src/util/STL/reverse_iterator.h"
dnl known to be present in gcc-4.1 headers, but missing in all previous versions
AC_DEFUN([AC_CXX_STL_REVERSE_ITERATOR_COMPARISONS],
[AC_REQUIRE([AC_PROG_CXX])	dnl from "config/cxx.m4"
AC_CACHE_CHECK(
	[whether libstdc++ (STL) already contains reverse_iterator comparisons],
[ac_cv_cxx_stl_reverse_iterator_comparisons],
[AC_LANG_PUSH(C++)
dnl saved_CXXFLAGS=$CXXFLAGS
dnl CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
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
dnl CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
if test "$ac_cv_cxx_stl_reverse_iterator_comparisons" = "yes" ; then
AC_DEFINE(HAVE_STL_REVERSE_ITERATOR_COMPARISONS, [],
	[True if STL <iterator> header defines reverse_iterator comparisons])
fi
])

dnl
dnl Category: C++
dnl Checks to see if std::ifstream can open "/dev/stdin"
dnl AC_DEFINEs HAVE_STD_IFSTREAM_DEV_STDIN if successful.  
dnl
AC_DEFUN([AC_CXX_STD_IFSTREAM_DEV_STDIN],
[AC_REQUIRE([AC_PROG_CXX])
AC_CACHE_CHECK(
	[whether std::ifstream works with /dev/stdin],
	[ac_cv_cxx_std_ifstream_dev_stdin],
[AC_LANG_PUSH(C++)
ac_cv_cxx_std_ifstream_dev_stdin=no
AC_LINK_IFELSE(
	AC_LANG_PROGRAM([[
		#include <iostream>
		#include <fstream>
		#include <string>
		using namespace std;
	]], [[
		ifstream fcin("/dev/stdin");
		if (fcin) {
			string line;
			while (getline(fcin, line)) {
				cout << line << endl;
			}
		} else {
			cout << "You lose." << endl;
		}
	]]),
	[echo "Hello, world!" > conftest.in
	echo "That's all, folks!" >> conftest.in
	./conftest$ac_exeext < conftest.in > conftest.out 2>&1
	if diff conftest.in conftest.out > /dev/null 2>&1
	then
		ac_cv_cxx_std_ifstream_dev_stdin=yes
	fi
	rm -f conftest.in conftest.out
	],
	[]
)
AC_LANG_POP(C++)
])
if test "$ac_cv_cxx_std_ifstream_dev_stdin" = yes ; then
AC_DEFINE(HAVE_STD_IFSTREAM_DEV_STDIN, [], 
	[Define if std::ifstream(/dev/stdin) works])
fi
])

dnl
dnl Category: C++
dnl Checks to see if std::ofstream can open "/dev/stdout"
dnl AC_DEFINEs HAVE_STD_OFSTREAM_DEV_STDOUT if successful.  
dnl
AC_DEFUN([AC_CXX_STD_OFSTREAM_DEV_STDOUT],
[AC_REQUIRE([AC_PROG_CXX])
AC_CACHE_CHECK(
	[whether std::ofstream works with /dev/stdout],
	[ac_cv_cxx_std_ofstream_dev_stdout],
[AC_LANG_PUSH(C++)
ac_cv_cxx_std_ofstream_dev_stdout=no
AC_LINK_IFELSE(
	AC_LANG_PROGRAM([[
		#include <iostream>
		#include <fstream>
		#include <string>
		using namespace std;
	]], [[
		ofstream fcout("/dev/stdout");
		if (cin) {
			string line;
			while (getline(cin, line)) {
				fcout << line << endl;
			}
		} else {
			cout << "You lose." << endl;
		}
	]]),
	[echo "Hello, world!" > conftest.in
	echo "That's all, folks!" >> conftest.in
	./conftest$ac_exeext < conftest.in > conftest.out 2>&1
	if diff conftest.in conftest.out > /dev/null 2>&1
	then
		ac_cv_cxx_std_ofstream_dev_stdout=yes
	fi
	rm -f conftest.in conftest.out
	],
	[]
)
AC_LANG_POP(C++)
])
if test "$ac_cv_cxx_std_ofstream_dev_stdout" = yes ; then
AC_DEFINE(HAVE_STD_OFSTREAM_DEV_STDOUT, [], 
	[Define if std::ofstream(/dev/stdout) works])
fi
])

dnl
dnl Category: C++
dnl Checks to see if std::ofstream can open "/dev/stderr"
dnl AC_DEFINEs HAVE_STD_OFSTREAM_DEV_STDERR if successful.  
dnl
AC_DEFUN([AC_CXX_STD_OFSTREAM_DEV_STDERR],
[AC_REQUIRE([AC_PROG_CXX])
AC_CACHE_CHECK(
	[whether std::ofstream works with /dev/stderr],
	[ac_cv_cxx_std_ofstream_dev_stderr],
[AC_LANG_PUSH(C++)
ac_cv_cxx_std_ofstream_dev_stderr=no
AC_LINK_IFELSE(
	AC_LANG_PROGRAM([[
		#include <iostream>
		#include <fstream>
		#include <string>
		using namespace std;
	]], [[
		ofstream fcerr("/dev/stderr");
		if (cin) {
			string line;
			while (getline(cin, line)) {
				fcerr << line << endl;
			}
		} else {
			cerr << "You lose." << endl;
		}
	]]),
	[echo "Hello, world!" > conftest.in
	echo "That's all, folks!" >> conftest.in
	./conftest$ac_exeext < conftest.in > conftest.out 2>&1
	if diff conftest.in conftest.out > /dev/null 2>&1
	then
		ac_cv_cxx_std_ofstream_dev_stderr=yes
	fi
	rm -f conftest.in conftest.out
	],
	[]
)
AC_LANG_POP(C++)
])
if test "$ac_cv_cxx_std_ofstream_dev_stderr" = yes ; then
AC_DEFINE(HAVE_STD_OFSTREAM_DEV_STDERR, [], 
	[Define if std::ofstream(/dev/stderr) works])
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
AC_CHECK_HEADERS([ext/hash_map hash_map alt/hash_map])
dnl used mainly in "src/util/hash_specializations.h"
AC_CHECK_HEADERS([ext/stl_hash_fun.h ext/hash_fun.h stl_hash_fun.h])
AC_CHECK_HEADERS([ext/hash_set hash_set alt/hash_set])
AC_CHECK_HEADERS([ext/slist slist alt/slist])
AC_CHECK_HEADERS([ext/hashtable.h tr1/hashtable hashtable.h])
AC_CHECK_HEADERS([tr1/unordered_map tr1/unordered_set])
AC_CHECK_HEADERS([ext/new_allocator.h])
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

dnl
dnl Detect the home namespace of hash_map.
dnl The location/existence of the header file does NOT necessarily correlate
dnl with the home namespace!  (Lesson learned)
dnl
AC_DEFUN([FANG_HASH_MAP_NAMESPACE],
[AC_REQUIRE([AC_HEADER_STDCXX])

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
])


dnl
dnl Checks whether or not the hash_map has 4 or 5 template arguments.
dnl SGI-style uses separate hash and compare functors.
dnl Intel style uses a composed hash_compare binary functor.  
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
	AC_DEFINE(HASH_MAP_SGI_STYLE, [],
		[Define if hash_map is SGI-style])
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
	AC_DEFINE(HASH_MAP_INTEL_STYLE, [],
		[Define if hash_map is Intel-style])
fi
])

dnl
dnl Three tests combined in one.
dnl Checks if <functional> contains _Identity, _Select1st, _Select2nd.
dnl
AC_DEFUN([FANG_CXX_STL_FUNCTIONAL_SELECT],
[AC_REQUIRE([AC_HEADER_STDCXX])

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

])


