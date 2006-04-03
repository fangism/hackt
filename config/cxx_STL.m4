dnl "config/cxx_STL.m4"
dnl	$Id: cxx_STL.m4,v 1.3 2006/04/03 05:30:33 fang Exp $
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
AC_CHECK_HEADERS([ext/hash_map hash_map])
dnl used mainly in "src/util/hash_specializations.h"
AC_CHECK_HEADERS([ext/stl_hash_fun.h ext/hash_fun.h stl_hash_fun.h])
AC_CHECK_HEADERS([ext/hash_set hash_set])
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

