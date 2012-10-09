dnl "config/cxx.m4"
dnl	$Id: cxx.m4,v 1.14 2011/02/08 22:32:44 fang Exp $
dnl autoconf macros for detecting characteristics of the C++ compiler.
dnl

dnl @synopsis _TRIVIAL_SOURCE_
dnl
dnl A dummy source file for trivial compiler tests.
dnl This is only useful for AC_COMPILE_IFLSE because it doesn't link.  
dnl
dnl @category Cxx
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
dnl
AC_DEFUN([_TRIVIAL_SOURCE_],
[AC_LANG_PROGRAM([extern int __foo__;])]
dnl [AC_LANG_PROGRAM([static int __foo__ = 4;],[while(__foo__) { --__foo__; }])]
)

dnl @synopsis FANG_ANAL_COMPILE_FLAGS
dnl
dnl This checks to see if your compilers understand the strictest
dnl warning flags, preferred by Fang.  
dnl The result is stored in ANAL_FLAGS.  
dnl For now, it just checks a fixed set of flags, and errors out
dnl if your compilers don't accept them.
dnl Some tests in the other macros in this project require that
dnl warnings be converted to errors.  
dnl Could rewrite this using AC_TRY_COMPILE.
dnl All other warning options can be cancelled with CFLAGS/CXXFLAGS
dnl e.g. -w cancels all warnings (gcc), 
dnl and -Wno-error cancel error-promotion (gcc).  
dnl
dnl TODO: workaround variations from other compilers.  
dnl NOTE: we check for gcc *last* because some other compiler (icc)
dnl are able to pass themselves off as the GNU C++ compiler in the 
dnl AC_PROG_CXX test, oddly enough... a bug?
dnl
dnl Depends on _TRIVIAL_SOURCE_
dnl Requires: FANG_CXX_COMPILER
dnl
dnl @category Cxx
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_ANAL_COMPILE_FLAGS],
[AC_REQUIRE([AC_PROG_CC])
AC_REQUIRE([FANG_CXX_COMPILER])
AC_ARG_ENABLE(strict-dialect,
	AS_HELP_STRING([--disable-strict-dialect],
	[Disables -ansi -pedantic-errors compile flags (default=enabled)
	This is sometimes necessary to allow 64b builds.])
)
if test x"$enable_strict_dialect" != xno ; then
	if test "$hackt_cxx_compiler_intel" = yes ; then
		TRY_DIALECT_FLAGS="-strict-ansi"
	elif test "$ac_cv_cxx_compiler_gnu" = yes ; then
		TRY_DIALECT_FLAGS="-ansi -pedantic-errors"
	else
		TRY_DIALECT_FLAGS="-ansi"
	fi
fi

AC_MSG_CHECKING([whether C/C++ compilers accept fangism's anal-retentive flags])
dnl icc diagnostic 561: nonstandard proprocessing directive (trouble w/ ccache)
if test "$hackt_cxx_compiler_intel" = yes ; then
	ANAL_FLAGS="$TRY_DIALECT_FLAGS -Wall -Werror -wd561 -wd1419"
elif test "$ac_cv_cxx_compiler_gnu" = yes ; then
	ANAL_FLAGS="$TRY_DIALECT_FLAGS -W -Wall -Werror"
	dnl try -Wextra later, below...
else
	ANAL_FLAGS="$TRY_DIALECT_FLAGS -Wall -Werror"
fi

AC_LANG_PUSH(C)
saved_CFLAGS="$CFLAGS"
CFLAGS="$ANAL_FLAGS"
AC_COMPILE_IFELSE([_TRIVIAL_SOURCE_],[],
[AC_MSG_ERROR([Your C compiler doesn't like flags: $ANAL_FLAGS
	Bug fangism about supporting your compiler.])]
)
CFLAGS="$saved_CFLAGS"
AC_LANG_POP(C)
AC_LANG_PUSH(C++)
saved_CXXFLAGS="$CXXFLAGS"
CXXFLAGS="$ANAL_FLAGS"
AC_COMPILE_IFELSE([_TRIVIAL_SOURCE_],[],
[AC_MSG_ERROR([Your C++ compiler doesn't like flags: $ANAL_FLAGS
	Bug fangism about supporting your compiler.])]
)
CXXFLAGS="$saved_CXXFLAGS"
AC_LANG_POP(C++)
dnl rm -f conftest.c conftest.cc
AC_MSG_RESULT([yes])
])dnl

dnl
dnl @synopsis FANG_CXXFLAGS_AUTO_NO_LONG_LONG
dnl
dnl Kludge: some autoconf tests for long long will unexpectedly pass
dnl even with strict ISO flags -ansi -pedantic-errors because some 
dnl system headers protect the use of long long.
dnl This macro detects the inconsistency and determines when 
dnl -Wno-long-long should be gratuitously appended to CFLAGS
dnl caveat: this flag is only known to be applicable for GNU gcc.
dnl
dnl Requires: already checked sizeof long long
dnl
AC_DEFUN([FANG_CXXFLAGS_AUTO_NO_LONG_LONG],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_CHECK_SIZEOF(long long)dnl may be already cached
if test x"$enable_strict_dialect" != xno ; then
	AC_MSG_CHECKING([whether long long is consistent with strict dialect])
if test "$ac_cv_sizeof_long_long" != 0 ; then
	AC_MSG_RESULT(no)
	AC_MSG_WARN([sizeof(long long) conflicts with strict C++ dialect!])
	AC_MSG_WARN([Gratuitously appending -Wno-long-long to CXXFLAGS...])
	CXXFLAGS="$CXXFLAGS -Wno-long-long"
else
	AC_MSG_RESULT(yes)
fi
fi
])dnl

dnl @synopsis FANG_STD_HEADERS_ANALLY_STRICT
dnl
dnl Check for whether or not standard library C headers pass
dnl the anal compile flags.  If they don't we have trouble... error out.  
dnl
dnl Requires: FANG_AM_FLAGS, FANG_CONFTEST_FLAGS, FANG_HEADER_STDCXX
dnl
dnl @category Cxx
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_STD_HEADERS_ANALLY_STRICT],
[AC_REQUIRE([FANG_AM_FLAGS])
AC_REQUIRE([FANG_HEADER_STDCXX])
AC_REQUIRE([FANG_CONFTEST_FLAGS])
AC_CACHE_CHECK([whether standard C++ headers are anally strictness-conforming],
[fang_cv_cxx_strict_anal_headers],
[AC_LANG_PUSH(C++)
	saved_CXXFLAGS=$CXXFLAGS
	CXXFLAGS="$CONFTEST_CXXFLAGS"
	AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([
		/* most of these should be standard */
		#include <cstddef>
		#include <cstdlib>
		#include <cstdio>
		#include <cassert>
		#include <cmath>
		#ifdef	HAVE_CCTYPE
		#include <cctype>
		#endif
		#include <cerrno>
		#ifdef	HAVE_CFLOAT
		#include <cfloat>
		#endif
		#ifdef	HAVE_CLIMITS
		#include <climits>
		#endif
		#ifdef	HAVE_CLOCALE
		#include <clocale>
		#endif
		#ifdef	HAVE_CSIGNAL
		#include <csignal>
		#endif
		#ifdef	HAVE_CSETJMP
		#include <csetjmp>
		#endif
		#ifdef	HAVE_CSTDARG
		#include <cstdarg>
		#endif
		#ifdef	HAVE_CTIME
		#include <ctime>
		#endif
		#ifdef	HAVE_CWCHAR
		#include <cwchar>
		#endif
		#ifdef	HAVE_CWCTYPE
		#include <cwctype>
		#endif
		],[]),
		[fang_cv_cxx_strict_anal_headers=yes],
		[fang_cv_cxx_strict_anal_headers=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_strict_anal_headers" = no ; then
	echo "CONFTEST_CXXFLAGS: $CONFTEST_CXXFLAGS"
	AC_MSG_ERROR([You're in big trouble if the standard headers don't pass with the given warnings flags!])
fi
])dnl

dnl @synopsis FANG_AM_FLAGS
dnl
dnl Sets automake flags with a bunch of anal retentive warnings, 
dnl most of which are cancellable through CXXFLAGS and CFLAGS.  
dnl
dnl TODO: redefine this macro cleanly, instead of this monolithic beast
dnl TODO: cache values!
dnl Results in AC_SUBST variables:
dnl	FANG_WARN_FLAGS, FANG_WARN_CFLAGS, FANG_WARN_CXXFLAGS
dnl the NOWARN_FLAGS are applied only during configuration tests
dnl it is often necessary to disable some strict warnings that affect the
dnl outcome of configure tests.
dnl
dnl @category Cxx
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_AM_FLAGS],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
dnl setup the list of flags to try
if test "$hackt_cxx_compiler_intel" = yes ; then
	TRY_WARN_FLAGS="-Wall -Wshadow -Wmain -Wpointer-arith -Wdeprecated -Werror"
dnl -wd is icc's equivalent of -Wno
dnl 111: unreachable statement
dnl 279: constant controlling expression
dnl 383: value copied to temporary, reference to temporary used
dnl 444: base class non-virtual destructor
dnl 561: nonstandard preprocessing directive '# 123 "file.c"'
dnl 869: unused parameter
dnl 981: operands evaluated in unspecified order
dnl 1419: external declaration in primary source file (WTF?)
	TRY_WARN_FLAGS="$TRY_WARN_FLAGS -wd111 -wd279 -wd561 -wd869 -wd981 -wd1419"
	TRY_WARN_CFLAGS="-Wmissing-prototypes"
	TRY_WARN_CXXFLAGS="-Wabi -wd383 -wd444"
	TRY_NOWARN_FLAGS="-Wno-shadow"
	TRY_NOWARN_CFLAGS="-Wno-missing-prototypes"
	TRY_NOWARN_CXXFLAGS=""
elif test "$ac_cv_cxx_compiler_gnu" = yes ; then
	TRY_WARN_FLAGS="-W -Wextra -Wall -Wundef -Wshadow -Wno-unused-parameter"
	TRY_WARN_FLAGS="$TRY_WARN_FLAGS -Wpointer-arith -Wcast-qual"
	TRY_WARN_FLAGS="$TRY_WARN_FLAGS -Wcast-align -Wconversion -Werror"
	TRY_WARN_CFLAGS="-Wmissing-prototypes -Wstrict-prototypes"
	TRY_WARN_CFLAGS="$TRY_WARN_CFLAGS -Wbad-function-cast -Wnested-externs"
	TRY_WARN_CXXFLAGS="-Wold-style-cast -Woverloaded-virtual"
dnl	TRY_NOWARN_FLAGS="-Wno-unused -Wno-shadow -Wno-cast-qual -Wno-long-double"
	TRY_NOWARN_FLAGS="-Wno-unused -Wno-shadow -Wno-cast-qual"
	TRY_NOWARN_CFLAGS="-Wno-strict-prototypes -Wno-missing-prototypes"
	TRY_NOWARN_CXXFLAGS="-Wno-overloaded-virtual"
else
	TRY_WARN_FLAGS="-Whatever"
	TRY_WARN_CFLAGS=""
	TRY_WARN_CXXFLAGS=""
	TRY_NOWARN_FLAGS=""
	TRY_NOWARN_CFLAGS=""
	TRY_NOWARN_CXXFLAGS=""
fi

FANG_WARN_FLAGS=""
FANG_WARN_CFLAGS=""
FANG_WARN_CXXFLAGS=""
CONFTEST_NOWARN_FLAGS=""
CONFTEST_NOWARN_CFLAGS=""
CONFTEST_NOWARN_CXXFLAGS=""
FANG_DIALECT_FLAGS=""
dnl default to C in language tests
AC_LANG_PUSH(C)
for f in $TRY_WARN_FLAGS
do
	saved_CFLAGS=$CFLAGS
	saved_CXXFLAGS=$CXXFLAGS
	CFLAGS=$f
	CXXFLAGS=$f
	AC_MSG_CHECKING([whether C and C++ compilers accept flag $f])
	AC_COMPILE_IFELSE([_TRIVIAL_SOURCE_],
	[AC_LANG_PUSH(C++)
	AC_COMPILE_IFELSE([_TRIVIAL_SOURCE_],
		[AC_MSG_RESULT([yes])
		FANG_WARN_FLAGS="$FANG_WARN_FLAGS $f"],
		[AC_MSG_RESULT([no])
		AC_MSG_WARN([Your C++ compiler doesn't like flag: $f])]
	)
	AC_LANG_POP(C++)
	],[AC_MSG_RESULT([no])
	AC_MSG_WARN([Your C compiler doesn't like flag: $f])]
	)
	CFLAGS=$saved_CFLAGS
	CXXFLAGS=$saved_CXXFLAGS
done
for f in $TRY_NOWARN_FLAGS
do
	saved_CFLAGS=$CFLAGS
	saved_CXXFLAGS=$CXXFLAGS
	CFLAGS=$f
	CXXFLAGS=$f
	AC_MSG_CHECKING([whether C and C++ compilers accept un-flag $f])
	AC_COMPILE_IFELSE([_TRIVIAL_SOURCE_],
	[AC_LANG_PUSH(C++)
	AC_COMPILE_IFELSE([_TRIVIAL_SOURCE_],
		[AC_MSG_RESULT([yes])
		CONFTEST_NOWARN_FLAGS="$CONFTEST_NOWARN_FLAGS $f"],
		[AC_MSG_RESULT([no])
		AC_MSG_WARN([Your C++ compiler doesn't like un-flag: $f])]
	)
	AC_LANG_POP(C++)
	],[AC_MSG_RESULT([no])
	AC_MSG_WARN([Your C compiler doesn't like un-flag: $f])]
	)
	CFLAGS=$saved_CFLAGS
	CXXFLAGS=$saved_CXXFLAGS
done
for f in $TRY_DIALECT_FLAGS
do
	saved_CFLAGS=$CFLAGS
	saved_CXXFLAGS=$CXXFLAGS
	CFLAGS=$f
	CXXFLAGS=$f
	AC_MSG_CHECKING([whether C and C++ compilers accept flag $f])
	AC_COMPILE_IFELSE([_TRIVIAL_SOURCE_],
	[AC_LANG_PUSH(C++)
	AC_COMPILE_IFELSE([_TRIVIAL_SOURCE_],
		[AC_MSG_RESULT([yes])
		FANG_DIALECT_FLAGS="$FANG_DIALECT_FLAGS $f"],
		[AC_MSG_RESULT([no])
		AC_MSG_WARN([Your C++ compiler doesn't like flag: $f])]
	)
	AC_LANG_POP(C++)
	],[AC_MSG_RESULT([no])
	AC_MSG_WARN([Your C compiler doesn't like flag: $f])]
	)
	CFLAGS=$saved_CFLAGS
	CXXFLAGS=$saved_CXXFLAGS
done
for f in $TRY_WARN_CFLAGS
do
	saved_CFLAGS=$CFLAGS
	CFLAGS=$f
	AC_MSG_CHECKING([whether C compiler accepts flag $f])
	AC_COMPILE_IFELSE([_TRIVIAL_SOURCE_],
	[AC_MSG_RESULT([yes])
	FANG_WARN_CFLAGS="$FANG_WARN_CFLAGS $f"],
	[AC_MSG_RESULT([no])
	AC_MSG_WARN([Your C compiler doesn't like flag: $f])]
	)
	CFLAGS=$saved_CFLAGS
done
for f in $TRY_NOWARN_CFLAGS
do
	saved_CFLAGS=$CFLAGS
	CFLAGS=$f
	AC_MSG_CHECKING([whether C compiler accepts un-flag $f])
	AC_COMPILE_IFELSE([_TRIVIAL_SOURCE_],
	[AC_MSG_RESULT([yes])
	CONFTEST_NOWARN_CFLAGS="$CONFTEST_NOWARN_CFLAGS $f"],
	[AC_MSG_RESULT([no])
	AC_MSG_WARN([Your C compiler doesn't like un-flag: $f])]
	)
	CFLAGS=$saved_CFLAGS
done
AC_LANG_POP(C)
AC_LANG_PUSH(C++)
for f in $TRY_WARN_CXXFLAGS
do
	saved_CXXFLAGS=$CXXFLAGS
	CXXFLAGS=$f
	AC_MSG_CHECKING([whether C++ compiler accepts flag $f])
	AC_COMPILE_IFELSE([_TRIVIAL_SOURCE_],
	[AC_MSG_RESULT([yes])
	FANG_WARN_CXXFLAGS="$FANG_WARN_CXXFLAGS $f"],
	[AC_MSG_RESULT([no])
	AC_MSG_WARN([Your C++ compiler doesn't like flag: $f])]
	)
	CXXFLAGS=$saved_CXXFLAGS
done
for f in $TRY_NOWARN_CXXFLAGS
do
	saved_CXXFLAGS=$CXXFLAGS
	CXXFLAGS=$f
	AC_MSG_CHECKING([whether C++ compiler accepts un-flag $f])
	AC_COMPILE_IFELSE([_TRIVIAL_SOURCE_],
	[AC_MSG_RESULT([yes])
	CONFTEST_NOWARN_CXXFLAGS="$CONFTEST_NOWARN_CXXFLAGS $f"],
	[AC_MSG_RESULT([no])
	AC_MSG_WARN([Your C++ compiler doesn't like un-flag: $f])]
	)
	CXXFLAGS=$saved_CXXFLAGS
done
AC_LANG_POP(C++)
dnl AC_MSG_NOTICE([general warning flags: $FANG_WARN_FLAGS])
dnl AC_MSG_NOTICE([general C warning flags: $FANG_WARN_CFLAGS])
dnl AC_MSG_NOTICE([general C++ warning flags: $FANG_WARN_CXXFLAGS])
dnl AC_MSG_NOTICE([general C/C++ dialect flags: $FANG_DIALECT_FLAGS])
AC_SUBST(FANG_WARN_FLAGS)
AC_SUBST(FANG_WARN_CFLAGS)
AC_SUBST(FANG_WARN_CXXFLAGS)
AC_SUBST(FANG_DIALECT_FLAGS)
])dnl


dnl @synopsis FANG_CXX_VERSION
dnl
dnl Produces an AC_SUBST-itutable string for the compiler version.
dnl Also warns if compiler version is detected as prerelease or experimental.  
dnl
dnl @category Cxx
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_VERSION],
[AC_REQUIRE([AC_PROG_CXX])
CXX_VERSION=`$CXX --version | head -n 1`
if ( echo "$CXX_VERSION" | grep -i prerelease )
then
	AC_MSG_WARN([
	Detected prerelease version of compiler.  No PRERELEASE compilers
	are officially supported.  Use at your own risk.])
elif ( echo "$CXX_VERSION" | grep -i experiment )
then
	AC_MSG_WARN([
	Detected experimental version of compiler.  No EXPERIMENTAL compilers
	are officially supported.  Use at your own risk.])
fi
AC_SUBST(CXX_VERSION)
])dnl

dnl @synopsis FANG_CXX_COMPILER
dnl
dnl Detects GNU C++ compiler.  
dnl Produces AM_CONDITIONAL variable HAVE_GXX
dnl Detects Intel C++ compiler.  
dnl Produces AM_CONDITIONAL variable HAVE_ICC
dnl and defined shell variable ac_cv_cxx_compiler_intel
dnl TODO: use AC_CACHE_CHECK
dnl Defines HAVE_GXX if compiler is GNU (or GNU-like)
dnl Defines HAVE_ICC if compiler is Intel's
dnl
dnl @category Cxx
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_COMPILER],
[AC_REQUIRE([FANG_CXX_VERSION])
AM_CONDITIONAL(HAVE_GXX, test x"$ac_cv_cxx_compiler_gnu" = "xyes")
if ( echo "$CXX_VERSION" | grep -i ICC )
then
	hackt_cxx_compiler_intel=yes
	AC_DEFINE(INTEL_COMPILER, [], [Define if Intel C/C++ compiler detected])
else
	hackt_cxx_compiler_intel=no
fi
AM_CONDITIONAL(HAVE_ICC, echo "$CXX_VERSION" | grep ICC)
])


dnl @synopsis FANG_CXX_DECLTYPE
dnl
dnl Checks for compiler support for decltype().
dnl NOTE: This check uses the C++ language mode.  
dnl Defines HAVE_DECLTYPE if operator is supported.
dnl
dnl @category Cxx
dnl @version 2010-09-15
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_DECLTYPE],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_CACHE_CHECK([whether compiler accepts decltype() operator],
[fang_cv_cxx_decltype],
[AC_LANG_PUSH(C++)
	AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([],[const int foo = 0; 
		const decltype(foo) bar = foo;
		return bar;]),
		[fang_cv_cxx_decltype=yes],
		[fang_cv_cxx_decltype=no]
	)
AC_LANG_POP(C++)
])
dnl AC_MSG_RESULT([$ac_cx_cxx_attribute_unused])
if test "$fang_cv_cxx_decltype" = "yes"; then
	AC_DEFINE(HAVE_DECLTYPE, [], 
		[True if compiler supports decltype() operator])
fi
])dnl

dnl @synopsis FANG_CXX_ATTRIBUTE_UNUSED
dnl
dnl Checks for compiler support for __attribute__((unused)).
dnl NOTE: This check uses the C++ language mode.  
dnl Defines HAVE_ATTRIBUTE_UNUSED if attribute is supported.
dnl
dnl @category Cxx
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_ATTRIBUTE_UNUSED],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_CACHE_CHECK([whether compiler accepts __attribute__((unused))],
[fang_cv_cxx_attribute_unused],
[AC_LANG_PUSH(C++)
	saved_CXXFLAGS=$CXXFLAGS
	CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
	AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([],[const int foo __attribute__ ((unused)) = 0;]),
		[fang_cv_cxx_attribute_unused=yes],
		[fang_cv_cxx_attribute_unused=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
dnl AC_MSG_RESULT([$ac_cx_cxx_attribute_unused])
if test "$fang_cv_cxx_attribute_unused" = "yes"; then
	AC_DEFINE(HAVE_ATTRIBUTE_UNUSED, [], 
		[True if compiler supports __attribute__((unused)) ])
fi
])dnl

dnl @synopsis FANG_CXX_ATTRIBUTE_UNUSED_CTOR
dnl
dnl Checks for where the unused attribute belongs around constructor syntax.
dnl Defines HAVE_ATTRIBUTE_UNUSED_BEFORE_CTOR or
dnl HAVE_ATTRIBUTE_UNUSED_AFTER_CTOR
dnl
dnl @category Cxx
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_ATTRIBUTE_UNUSED_CTOR],
[AC_REQUIRE([FANG_CXX_ATTRIBUTE_UNUSED])
AC_LANG_PUSH(C++)
saved_CXXFLAGS=$CXXFLAGS
CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
AC_CACHE_CHECK([if __attribute__((unused)) comes before construction],
[fang_cv_cxx_attribute_unused_before_ctor],
[AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([class foo { public: foo(const int); };],
		[const foo bar __attribute__ ((unused)) (13);]),
		[fang_cv_cxx_attribute_unused_before_ctor=yes],
		[fang_cv_cxx_attribute_unused_before_ctor=no]
	)
])
dnl AC_MSG_RESULT([$ac_cx_cxx_attribute_unused_before_ctor])
if test "$fang_cv_cxx_attribute_unused_before_ctor" = "yes"; then
AC_DEFINE(HAVE_ATTRIBUTE_UNUSED_BEFORE_CTOR, [],
	[True if __attribute__((unused)) is accepted before construction])
fi

AC_CACHE_CHECK([if __attribute__((unused)) comes after construction],
[fang_cv_cxx_attribute_unused_after_ctor],
[AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([class foo { public: foo(const int); };],
		[const foo bar (13) __attribute__ ((unused));]),
		[fang_cv_cxx_attribute_unused_after_ctor=yes],
		[fang_cv_cxx_attribute_unused_after_ctor=no]
	)
])
dnl AC_MSG_RESULT([$ac_cx_cxx_attribute_unused_after_ctor])
if test "$fang_cv_cxx_attribute_unused_after_ctor" = "yes"; then
AC_DEFINE(HAVE_ATTRIBUTE_UNUSED_AFTER_CTOR, [],
	[True if __attribute__((unused)) is accepted after construction])
fi

CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])dnl

dnl @synopsis FANG_CXX_ATTRIBUTE_CONST
dnl
dnl Check for __attribute__ ((const))
dnl Define HAVE_ATTRIBUTE_CONST if supported.  
dnl
dnl @category Cxx
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_ATTRIBUTE_CONST],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_REQUIRE([FANG_STD_HEADERS_ANALLY_STRICT])
AC_CACHE_CHECK([whether compiler accepts __attribute__((const))],
[fang_cv_cxx_attribute_const],
[AC_LANG_PUSH(C++)
	saved_CXXFLAGS=$CXXFLAGS
	CXXFLAGS="$CONFTEST_CXXFLAGS"
	AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([#include <cstdlib>
		void pure_func (void) __attribute__ ((const));
		void pure_func (void) { exit (1); }
		],[pure_func();]),
		[fang_cv_cxx_attribute_const=yes],
		[fang_cv_cxx_attribute_const=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_attribute_const" = "yes"; then
AC_DEFINE(HAVE_ATTRIBUTE_CONST, [],
	[True if compiler supports __attribute__((const)) ])
fi
])dnl

dnl @synopsis FANG_CXX_ATTRIBUTE_PURE
dnl
dnl Check for __attribute__ ((pure))
dnl 'pure' marks a function as having no side-effects.  
dnl Define HAVE_ATTRIBUTE_PURE if supported.  
dnl
dnl @category Cxx
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_ATTRIBUTE_PURE],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_REQUIRE([FANG_STD_HEADERS_ANALLY_STRICT])
AC_CACHE_CHECK([whether compiler accepts __attribute__((pure))],
[fang_cv_cxx_attribute_pure],
[AC_LANG_PUSH(C++)
	saved_CXXFLAGS=$CXXFLAGS
	CXXFLAGS="$CONFTEST_CXXFLAGS"
	AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([#include <cstdlib>
		void pure_func (void) __attribute__ ((pure));
		void pure_func (void) { exit (1); }
		],[pure_func();]),
		[fang_cv_cxx_attribute_pure=yes],
		[fang_cv_cxx_attribute_pure=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_attribute_pure" = "yes"; then
AC_DEFINE(HAVE_ATTRIBUTE_PURE, [],
	[True if compiler supports __attribute__((pure)) ])
fi
])dnl

dnl @synopsis FANG_CXX_ATTRIBUTE_PURE
dnl
dnl Check for __attribute__ ((holy))
dnl As far as I know this is not a real attribute, so this is useful for
dnl making sure that conftests do fail on unknown attributes.  
dnl Define HAVE_ATTRIBUTE_PURE if supported.  
dnl
dnl @category Cxx
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_ATTRIBUTE_HOLY],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_REQUIRE([FANG_STD_HEADERS_ANALLY_STRICT])
AC_CACHE_CHECK([whether compiler accepts __attribute__((holy))],
[fang_cv_cxx_attribute_holy],
[AC_LANG_PUSH(C++)
	saved_CXXFLAGS=$CXXFLAGS
	CXXFLAGS="$CONFTEST_CXXFLAGS"
	AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([#include <cstdlib>
		void holy_func (void) __attribute__ ((holy));
		void holy_func (void) { exit (1); }
		],[holy_func();]),
		[fang_cv_cxx_attribute_holy=yes],
		[fang_cv_cxx_attribute_holy=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_attribute_holy" = "yes"; then
AC_DEFINE(HAVE_ATTRIBUTE_HOLY, [],
	[True if compiler supports __attribute__((holy)) ])
fi
])dnl

dnl @synopsis FANG_CXX_ATTRIBUTE_PRECIOUS, my preciousss...
dnl
dnl Check for __attribute__ ((precious))
dnl As far as I know this is not a real attribute, so this is useful for
dnl making sure that conftests do fail on unknown attributes.  
dnl Define HAVE_ATTRIBUTE_PRECIOUS if supported.  
dnl
dnl @category Cxx
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_ATTRIBUTE_PRECIOUS],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_REQUIRE([FANG_STD_HEADERS_ANALLY_STRICT])
AC_CACHE_CHECK([whether compiler accepts __attribute__((precious))],
[fang_cv_cxx_attribute_precious],
[AC_LANG_PUSH(C++)
	saved_CXXFLAGS=$CXXFLAGS
	CXXFLAGS="$CONFTEST_CXXFLAGS"
	AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([#include <cstdlib>
		void precious_func (void) __attribute__ ((precious));
		void precious_func (void) { exit (1); }
		],[precious_func();]),
		[fang_cv_cxx_attribute_precious=yes],
		[fang_cv_cxx_attribute_precious=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_attribute_precious" = "yes"; then
AC_DEFINE(HAVE_ATTRIBUTE_PRECIOUS, [],
	[True if compiler supports __attribute__((precious)) ])
fi
])dnl

dnl @synopsis FANG_CXX_ATTRIBUTE_EVIL
dnl
dnl Check for __attribute__ ((evil))
dnl Define HAVE_ATTRIBUTE_EVIL if evil.  
dnl
dnl @category Cxx
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_ATTRIBUTE_EVIL],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_REQUIRE([FANG_STD_HEADERS_ANALLY_STRICT])
AC_CACHE_CHECK([whether compiler accepts __attribute__((evil))],
[fang_cv_cxx_attribute_evil],
[AC_LANG_PUSH(C++)
	saved_CXXFLAGS=$CXXFLAGS
	CXXFLAGS="$CONFTEST_CXXFLAGS"
	AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([#include <cstdlib>
		void evil_func (void) __attribute__ ((evil));
		void evil_func (void) { exit (1); }
		],[evil_func();]),
		[fang_cv_cxx_attribute_evil=yes],
		[fang_cv_cxx_attribute_evil=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_attribute_evil" = "yes"; then
AC_DEFINE(HAVE_ATTRIBUTE_EVIL, [],
	[True if compiler supports __attribute__((evil)) ])
fi
])dnl

dnl @synopsis FANG_CXX_ATTRIBUTE_NORETURN
dnl
dnl Checks for __attribute__ ((noreturn))
dnl Define HAVE_ATTRIBUTE_NORETURN if supported.  
dnl
dnl @category Cxx
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_ATTRIBUTE_NORETURN],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_REQUIRE([FANG_STD_HEADERS_ANALLY_STRICT])
AC_CACHE_CHECK([whether compiler accepts __attribute__((noreturn))],
[fang_cv_cxx_attribute_noreturn],
[AC_LANG_PUSH(C++)
	saved_CXXFLAGS=$CXXFLAGS
	CXXFLAGS="$CONFTEST_CXXFLAGS"
	AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([#include <cstdlib>
		void die (int) __attribute__ ((noreturn));
		void die (int k) { exit (k); }],
		[die(1);]),
		[fang_cv_cxx_attribute_noreturn=yes],
		[fang_cv_cxx_attribute_noreturn=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_attribute_noreturn" = "yes"; then
AC_DEFINE(HAVE_ATTRIBUTE_NORETURN, [],
	[True if compiler supports __attribute__((noreturn)) ])
fi
])dnl

dnl @synopsis FANG_CXX_ATTRIBUTE_NORETURN
dnl
dnl Checks for __attribute__ ((malloc))
dnl 'malloc' says that function returns non-aliasing pointers.  
dnl Define HAVE_ATTRIBUTE_MALLOC if supported.  
dnl
dnl @category Cxx
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_ATTRIBUTE_MALLOC],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_CACHE_CHECK([whether compiler accepts __attribute__((malloc))],
[fang_cv_cxx_attribute_malloc],
[AC_LANG_PUSH(C++)
	saved_CXXFLAGS=$CXXFLAGS
	CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
	AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([
		void* __fangmalloc (unsigned int) __attribute__ ((malloc));
		void __fangfree(void*);],
		[void* mem = __fangmalloc(4); __fangfree(mem);]),
		[fang_cv_cxx_attribute_malloc=yes],
		[fang_cv_cxx_attribute_malloc=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
dnl AC_MSG_RESULT([$fang_cv_cxx_attribute_malloc])
if test "$fang_cv_cxx_attribute_malloc" = "yes"; then
AC_DEFINE(HAVE_ATTRIBUTE_MALLOC, [],
	[True if compiler supports __attribute__((malloc)) ])
fi
])dnl

dnl @synopsis FANG_CXX_ATTRIBUTE_VISIBILITY
dnl
dnl Checking for visibility attributes, "hidden" and "default"
dnl New since gcc-4.0.
dnl Define HAVE_ATTRIBUTE_VISIBILITY_HIDDEN if supported.  
dnl Define HAVE_ATTRIBUTE_VISIBILITY_DEFAULT if supported.  
dnl
dnl @category Cxx
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_ATTRIBUTE_VISIBILITY],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_LANG_PUSH(C++)
saved_CXXFLAGS=$CXXFLAGS
CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
dnl check for __visibility__(("hidden"))
AC_CACHE_CHECK([whether compiler accepts __attribute__((visibility("hidden")))],
[fang_cv_cxx_attribute_visibility_hidden],
	AC_COMPILE_IFELSE(
		AC_LANG_PROGRAM(
		[struct __attribute__((visibility("hidden"))) foo {
			foo() { }
			~foo() { }
		};],
		[foo bar;]),
		[fang_cv_cxx_attribute_visibility_hidden=yes],
		[fang_cv_cxx_attribute_visibility_hidden=no]
	)
)
if test "$fang_cv_cxx_attribute_visibility_hidden" = "yes"; then
AC_DEFINE(HAVE_ATTRIBUTE_VISIBILITY_HIDDEN, [],
        [True if compiler supports __attribute__((visibility("hidden"))) ])
fi
dnl check for __visibility__(("default"))
AC_CACHE_CHECK([whether compiler accepts __attribute__((visibility("default")))],
[fang_cv_cxx_attribute_visibility_default],
	AC_COMPILE_IFELSE(
		AC_LANG_PROGRAM(
		[struct __attribute__((visibility("default"))) foo {
			foo() { }
			~foo() { }
		};],
		[foo bar;]),
		[fang_cv_cxx_attribute_visibility_default=yes],
		[fang_cv_cxx_attribute_visibility_default=no]
	)
)
if test "$fang_cv_cxx_attribute_visibility_default" = "yes"; then
AC_DEFINE(HAVE_ATTRIBUTE_VISIBILITY_DEFAULT, [],
        [True if compiler supports __attribute__((visibility("default"))) ])
fi
dnl restore flags and language
CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])dnl


dnl @synopsis FANG_CXX_ATTRIBUTE_PACKED
dnl
dnl Checking for packed attribute.
dnl Define HAVE_ATTRIBUTE_PACKED if supported.  
dnl
dnl @category Cxx
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_ATTRIBUTE_PACKED],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_CACHE_CHECK([whether compiler accepts __attribute__((packed))],
[fang_cv_cxx_attribute_packed],
[AC_LANG_PUSH(C++)
	saved_CXXFLAGS=$CXXFLAGS
	CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
	AC_COMPILE_IFELSE(
		AC_LANG_PROGRAM(
		[struct __attribute__((packed)) foo {
			char x; char y; char z;
			foo() { }
			~foo() { }
		};],
		[foo bar;]),
		[fang_cv_cxx_attribute_packed=yes],
		[fang_cv_cxx_attribute_packed=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_attribute_packed" = "yes"; then
AC_DEFINE(HAVE_ATTRIBUTE_PACKED, [],
        [True if compiler supports __attribute__((packed)) ])
fi
])dnl

dnl @synopsis FANG_CXX_ATTRIBUTE_ALIGNED
dnl
dnl Checking for aligned attribute.
dnl Define HAVE_ATTRIBUTE_ALIGNED if supported.  
dnl
dnl @category Cxx
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_ATTRIBUTE_ALIGNED],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_CACHE_CHECK([whether compiler accepts __attribute__((aligned))],
[fang_cv_cxx_attribute_aligned],
[AC_LANG_PUSH(C++)
	saved_CXXFLAGS=$CXXFLAGS
	CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
	AC_COMPILE_IFELSE(
		AC_LANG_PROGRAM(
		[struct __attribute__((aligned)) foo {
			char x; char y; char z;
			foo() { }
			~foo() { }
		};],
		[foo bar;]),
		[fang_cv_cxx_attribute_aligned=yes],
		[fang_cv_cxx_attribute_aligned=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_attribute_aligned" = "yes"; then
AC_DEFINE(HAVE_ATTRIBUTE_ALIGNED, [],
        [True if compiler supports __attribute__((aligned)) ])
fi
])dnl

dnl @synopsis FANG_CXX_ATTRIBUTE_ALIGNED_SIZE
dnl
dnl Checking for aligned (size) attribute.
dnl Define HAVE_ATTRIBUTE_ALIGNED_SIZE if supported.  
dnl
dnl @category Cxx
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_ATTRIBUTE_ALIGNED_SIZE],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_CACHE_CHECK([whether compiler accepts __attribute__((aligned(size)))],
[fang_cv_cxx_attribute_aligned_size],
[AC_LANG_PUSH(C++)
	saved_CXXFLAGS=$CXXFLAGS
	CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
	AC_COMPILE_IFELSE(
		AC_LANG_PROGRAM(
		[struct __attribute__((aligned(8))) foo {
			char x; char y; char z;
			foo() { }
			~foo() { }
		};],
		[foo bar;]),
		[fang_cv_cxx_attribute_aligned_size=yes],
		[fang_cv_cxx_attribute_aligned_size=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_attribute_aligned_size" = "yes"; then
AC_DEFINE(HAVE_ATTRIBUTE_ALIGNED_SIZE, [],
        [True if compiler supports __attribute__((aligned(size))) ])
fi
])dnl


dnl @synopsis FANG_CXX_DEFAULT_TEMPLATE_TEMPLATE_PARAMETER_BINDING
dnl
dnl Checking for extension that allows binding of templates with
dnl fewer arguments to template with more parameters and sufficient
dnl trailing default parameters.  
dnl See: http://gcc.gnu.org/gcc-4.2/changes.html
dnl	which is where test case is stolen from.  
dnl gcc-4.2 is the first branch series that rejects this.  
dnl Define HAVE_DEFAULT_TEMPLATE_TEMPLATE_PARAMETER_BINDING
dnl
dnl @category Cxx
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_DEFAULT_TEMPLATE_TEMPLATE_PARAMETER_BINDING],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_CACHE_CHECK(
[whether templates with default parameters can bind to template template parameters with fewer parameters],
[fang_cv_cxx_template_template_parameter_default_binding],
[AC_LANG_PUSH(C++)
	saved_CXXFLAGS=$CXXFLAGS
	CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
	AC_COMPILE_IFELSE(
		AC_LANG_PROGRAM(
		[[template <template <typename> class C>
		void f(C<double>) {}
		template <typename T, typename U = int>
		struct S {};
		template void f(S<double>);
		]], []),
		[fang_cv_cxx_template_template_parameter_default_binding=yes],
		[fang_cv_cxx_template_template_parameter_default_binding=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_template_template_parameter_default_binding" = "yes"; then
AC_DEFINE(HAVE_DEFAULT_TEMPLATE_TEMPLATE_PARAMETER_BINDING, [],
        [Define if templates with default arguments can bind to template template parameters with fewer parameters.])
fi
])dnl


dnl @synopsis FANG_CXX_TEMPLATE_FORMAL_BASE_CLASS
dnl
dnl Checking whether or not template parameter may be named directly 
dnl as a base type.  
dnl The work around is to use the identity<> type-trait "util/type_traits.h"
dnl to create an indirect reference to the desired type.  
dnl Define HAVE_TEMPLATE_FORMAL_BASE_CLASS.
dnl
dnl @category Cxx
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_TEMPLATE_FORMAL_BASE_CLASS],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_CACHE_CHECK(
[whether templates formal parameter may be named directly as base class],
[fang_cv_cxx_template_formal_base_class],
[AC_LANG_PUSH(C++)
	saved_CXXFLAGS=$CXXFLAGS
	CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
	AC_COMPILE_IFELSE(
		AC_LANG_PROGRAM(
		[template <class C>
		struct S : public C { };
		], []),
		[fang_cv_cxx_template_formal_base_class=yes],
		[fang_cv_cxx_template_formal_base_class=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_template_formal_base_class" = "yes"; then
AC_DEFINE(HAVE_TEMPLATE_FORMAL_BASE_CLASS, [],
        [Define if templates formal parameters may be named as base classes.])
fi
])dnl

dnl @synopsis FANG_CXX_FRIEND_FUNCTION_NAMESPACE
dnl
dnl Checking whether or not friend function declaration needs to
dnl be qualified with its home namespace.
dnl Define FRIEND_FUNCTION_HOME_NAMESPACE.
dnl
dnl @category Cxx
dnl @version 2012-10-09
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_FRIEND_FUNCTION_NAMESPACE],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_CACHE_CHECK(
[whether friend functions may be declared with their home namespace],
[fang_cv_cxx_friend_function_home_namespace_allowed],
[AC_LANG_PUSH(C++)
	saved_CXXFLAGS=$CXXFLAGS
	CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
	AC_COMPILE_IFELSE(
		AC_LANG_PROGRAM(
		[[
namespace std {
	template <class _T1>
	inline void _Construct(_T1*);
	template <class _T1, class _T2>
	inline void _Construct(_T1*, const _T2&);
}
using std::_Construct;
class thing {
	typedef thing				this_type;
#define FRIEND_NAMESPACE_CONSTRUCT		std::
	friend void FRIEND_NAMESPACE_CONSTRUCT
		_Construct<this_type>(this_type*);
	friend void FRIEND_NAMESPACE_CONSTRUCT
		_Construct<this_type, this_type>(
		this_type* __p, const this_type& __value);
};
		]], []),
		[fang_cv_cxx_friend_function_home_namespace_allowed=yes],
		[fang_cv_cxx_friend_function_home_namespace_allowed=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
AC_CACHE_CHECK(
[whether friend functions must be declared with their home namespace],
[fang_cv_cxx_friend_function_home_namespace_required],
[AC_LANG_PUSH(C++)
	saved_CXXFLAGS=$CXXFLAGS
	CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
	AC_COMPILE_IFELSE(
		AC_LANG_PROGRAM(
		[[
namespace std {
        template <class _T1>
        inline void _Construct(_T1*);
        template <class _T1, class _T2>
        inline void _Construct(_T1*, const _T2&);
}
using std::_Construct;
class thing {
        typedef thing		this_type;
#define FRIEND_NAMESPACE_CONSTRUCT
        friend void FRIEND_NAMESPACE_CONSTRUCT
                _Construct<this_type>(this_type*);
        friend void FRIEND_NAMESPACE_CONSTRUCT
                _Construct<this_type, this_type>(
                this_type* __p, const this_type& __value);
};
		]], []),
		[fang_cv_cxx_friend_function_home_namespace_required=no],
		[fang_cv_cxx_friend_function_home_namespace_required=yes]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_friend_function_home_namespace_allowed" = "yes"; then
AC_DEFINE(FRIEND_FUNCTION_HOME_NAMESPACE_ALLOWED, [],
        [Define if friend functions may be declared with their home namespace.])
fi
if test "$fang_cv_cxx_friend_function_home_namespace_required" = "yes"; then
AC_DEFINE(FRIEND_FUNCTION_HOME_NAMESPACE_REQUIRED, [],
        [Define if friend functions must be declared with their home namespace.])
fi
])dnl



dnl @synopsis FANG_CXX_OVERLOAD_VIRTUAL_USING
dnl
dnl Checking whether or not virtual functions allow/need explicit
dnl using directives to overload virtual functions.
dnl The workaround was intended for a difference between gcc and clang.
dnl Define OVERLOAD_VIRTUAL_USING_ALLOWED.
dnl Define OVERLOAD_VIRTUAL_USING_REQUIRED.
dnl
dnl @category Cxx
dnl @version 2012-10-09
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CXX_OVERLOAD_VIRTUAL_USING],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_CACHE_CHECK(
[whether virtual method overloads with using directives are required],
[fang_cv_cxx_overload_virtual_using_required],
[AC_LANG_PUSH(C++)
	saved_CXXFLAGS=$CXXFLAGS
	CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
	AC_COMPILE_IFELSE(
		AC_LANG_PROGRAM(
		[[
class pint_const;
class const_param;
class nonmeta_index_expr_base {
public:
virtual ~nonmeta_index_expr_base();
virtual const pint_const* deep_copy(const nonmeta_index_expr_base*) const = 0;
};
typedef nonmeta_index_expr_base meta_index_expr;
class data_expr {
public:
virtual ~data_expr();
virtual const const_param* deep_copy(const data_expr*) const = 0;
};
class int_expr : virtual public nonmeta_index_expr_base, public data_expr {
public:
virtual ~int_expr();
virtual const const_param* deep_copy(const int_expr*) const = 0;
 const const_param* deep_copy(const data_expr*) const;
 const pint_const* deep_copy(const nonmeta_index_expr_base*) const;
};
class pint_expr :
  virtual public meta_index_expr,
  public int_expr {
public:
virtual ~pint_expr();
 const const_param* deep_copy(const int_expr*) const;
protected:
 using int_expr::deep_copy;
// using meta_index_expr::deep_copy;      // clang wants, gcc doesn't
};
		]], []),
		[fang_cv_cxx_overload_virtual_using_required=no],
		[fang_cv_cxx_overload_virtual_using_required=yes]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
AC_CACHE_CHECK(
[whether virtual method overloads with using directives are allowed],
[fang_cv_cxx_overload_virtual_using_allowed],
[AC_LANG_PUSH(C++)
	saved_CXXFLAGS=$CXXFLAGS
	CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
	AC_COMPILE_IFELSE(
		AC_LANG_PROGRAM(
		[[
class pint_const;
class const_param;
class nonmeta_index_expr_base {
public:
virtual ~nonmeta_index_expr_base();
virtual const pint_const* deep_copy(const nonmeta_index_expr_base*) const = 0;
};
typedef nonmeta_index_expr_base meta_index_expr;
class data_expr {
public:
virtual ~data_expr();
virtual const const_param* deep_copy(const data_expr*) const = 0;
};
class int_expr : virtual public nonmeta_index_expr_base, public data_expr {
public:
virtual ~int_expr();
virtual const const_param* deep_copy(const int_expr*) const = 0;
 const const_param* deep_copy(const data_expr*) const;
 const pint_const* deep_copy(const nonmeta_index_expr_base*) const;
};
class pint_expr :
  virtual public meta_index_expr,
  public int_expr {
public:
virtual ~pint_expr();
 const const_param* deep_copy(const int_expr*) const;
protected:
 using int_expr::deep_copy;
 using meta_index_expr::deep_copy;      // clang wants, gcc doesn't
};
		]], []),
		[fang_cv_cxx_overload_virtual_using_allowed=yes],
		[fang_cv_cxx_overload_virtual_using_allowed=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
if test "$fang_cv_cxx_overload_virtual_using_required" = "yes"; then
AC_DEFINE(OVERLOAD_VIRTUAL_USING_REQUIRED, [],
        [Define if overloading virtual functions with using is required.])
fi
if test "$fang_cv_cxx_overload_virtual_using_allowed" = "yes"; then
AC_DEFINE(OVERLOAD_VIRTUAL_USING_ALLOWED, [],
        [Define if overloading virtual functions with using is allowed.])
fi
])dnl

