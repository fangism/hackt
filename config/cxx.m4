dnl "config/cxx.m4"
dnl	$Id: cxx.m4,v 1.5.2.1 2006/04/24 05:42:14 fang Exp $
dnl autoconf macros for detecting characteristics of the C++ compiler.
dnl

dnl
dnl dummy source file for trivial compiler tests
dnl This is only useful for AC_COMPILE_IFLSE because it doesn't link.  
dnl
AC_DEFUN([_TRIVIAL_SOURCE_],
[AC_LANG_PROGRAM([extern int __foo__;])]
dnl [AC_LANG_PROGRAM([static int __foo__ = 4;],[while(__foo__) { --__foo__; }])]
)

dnl
dnl This checks to see if your compilers understand the strictest
dnl warning flags.  
dnl The result is stored in ANAL_FLAGS.  
dnl For now, it just checks a fixed set of flags, and errors out
dnl if your compilers don't accept them.
dnl TODO: workaround variations from other compilers.  
dnl Some tests in the other macros in this project require that
dnl warnings be converted to errors.  
dnl Could rewrite this using AC_TRY_COMPILE.
dnl Once -ansi is enabled there's no other option to cancel it out
dnl thus we introduce a configure switch to disable it.  
dnl All other warning options can be cancelled with CFLAGS/CXXFLAGS
dnl e.g. -w camcels all warnings, and -Wno-error cancel error-promotion.  
dnl
dnl NOTE: we check for gcc last because some other compiler (icc)
dnl are able to pass themselves off as the GNU C++ compiler in the 
dnl AC_PROG_CXX test, oddly enough... a bug?
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
])

dnl
dnl Check for whether or not standard library headers pass
dnl the anal compile flags.  If they don't we have trouble...
dnl
AC_DEFUN([FANG_STD_HEADERS_ANALLY_STRICT],
[AC_REQUIRE([FANG_AM_FLAGS])
AC_REQUIRE([AC_HEADER_STDCXX])
AC_REQUIRE([FANG_CONFTEST_FLAGS])
AC_CACHE_CHECK([whether standard C++ headers are anally strictness-conforming],
[fang_cv_cxx_strict_anal_headers],
[AC_LANG_PUSH(C++)
	saved_CXXFLAGS=$CXXFLAGS
	dnl CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
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
dnl AC_MSG_RESULT([$fang_cv_cxx_strict_anal_headers])
if test "$fang_cv_cxx_strict_anal_headers" = no ; then
	AC_MSG_RESULT(no)
	AC_MSG_ERROR([You're in big trouble!])
else
	AC_MSG_RESULT(yes)
fi
])

dnl
dnl TODO: redefine this macro cleanly, instead of this monolithic beast
dnl TODO: learn m4
dnl TODO: cache values!
dnl Results in AC_SUBST variables:
dnl	FANG_WARN_FLAGS, FANG_WARN_CFLAGS, FANG_WARN_CXXFLAGS
dnl the NOWARN_FLAGS are applied only during configuration tests
dnl it is often necessary to disable some strict warnings that affect the
dnl outcome of configure tests
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
	TRY_WARN_FLAGS="-W -Wall -Wundef -Wshadow -Wno-unused-parameter"
	TRY_WARN_FLAGS="$TRY_WARN_FLAGS -Wpointer-arith -Wcast-qual"
	TRY_WARN_FLAGS="$TRY_WARN_FLAGS -Wcast-align -Wconversion -Werror"
	TRY_WARN_CFLAGS="-Wmissing-prototypes -Wstrict-prototypes"
	TRY_WARN_CFLAGS="$TRY_WARN_CFLAGS -Wbad-function-cast -Wnested-externs"
	TRY_WARN_CXXFLAGS="-Wold-style-cast -Woverloaded-virtual"
	TRY_NOWARN_FLAGS="-Wno-unused -Wno-missing-prototypes -Wno-shadow"
	TRY_NOWARN_FLAGS="$TRY_NOWARN_FLAGS -Wno-cast-qual -Wno-long-double"
	TRY_NOWARN_CFLAGS="-Wno-strict-prototypes"
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
])


dnl
dnl Produces an AC_SUBST-itutable string for the compiler version.
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
])

dnl
dnl Detects GNU C++ compiler.  
dnl Produces AM_CONDITIONAL variable HAVE_GXX
dnl Detects Intel C++ compiler.  
dnl Produces AM_CONDITIONAL variable HAVE_ICC
dnl and defined shell variable ac_cv_cxx_compiler_intel
dnl TODO: use AC_CACHE_CHECK
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


dnl
dnl Checks for compiler support for __attribute__((unused)).
dnl NOTE: This check uses the C++ language mode.  
dnl Result: AC_DEFINE(HAVE_ATTRIBUTE_UNUSED)
dnl
AC_DEFUN([AC_CXX_ATTRIBUTE_UNUSED],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_CACHE_CHECK([whether compiler accepts __attribute__((unused))],
[ac_cv_cxx_attribute_unused],
[AC_LANG_PUSH(C++)
	saved_CXXFLAGS=$CXXFLAGS
	CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
	AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([],[const int foo __attribute__ ((unused)) = 0;]),
		[ac_cv_cxx_attribute_unused=yes],
		[ac_cv_cxx_attribute_unused=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
dnl AC_MSG_RESULT([$ac_cx_cxx_attribute_unused])
if test "$ac_cv_cxx_attribute_unused" = "yes"; then
	AC_DEFINE(HAVE_ATTRIBUTE_UNUSED, [], 
		[True if compiler supports __attribute__((unused)) ])
fi
])

dnl
dnl Checks for where unused attribute belong around constructor syntax.
dnl
AC_DEFUN([AC_CXX_ATTRIBUTE_UNUSED_CTOR],
[AC_REQUIRE([AC_CXX_ATTRIBUTE_UNUSED])
AC_LANG_PUSH(C++)
saved_CXXFLAGS=$CXXFLAGS
CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
AC_CACHE_CHECK([if __attribute__((unused)) comes before construction],
[ac_cv_cxx_attribute_unused_before_ctor],
[AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([class foo { public: foo(const int); };],
		[const foo bar __attribute__ ((unused)) (13);]),
		[ac_cv_cxx_attribute_unused_before_ctor=yes],
		[ac_cv_cxx_attribute_unused_before_ctor=no]
	)
])
dnl AC_MSG_RESULT([$ac_cx_cxx_attribute_unused_before_ctor])
if test "$ac_cv_cxx_attribute_unused_before_ctor" = "yes"; then
AC_DEFINE(HAVE_ATTRIBUTE_UNUSED_BEFORE_CTOR, [],
	[True if __attribute__((unused)) is accepted before construction])
fi

AC_CACHE_CHECK([if __attribute__((unused)) comes after construction],
[ac_cv_cxx_attribute_unused_after_ctor],
[AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([class foo { public: foo(const int); };],
		[const foo bar (13) __attribute__ ((unused));]),
		[ac_cv_cxx_attribute_unused_after_ctor=yes],
		[ac_cv_cxx_attribute_unused_after_ctor=no]
	)
])
dnl AC_MSG_RESULT([$ac_cx_cxx_attribute_unused_after_ctor])
if test "$ac_cv_cxx_attribute_unused_after_ctor" = "yes"; then
AC_DEFINE(HAVE_ATTRIBUTE_UNUSED_AFTER_CTOR, [],
	[True if __attribute__((unused)) is accepted after construction])
fi

CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])

dnl
dnl Check for __attribute__ ((const))
dnl
AC_DEFUN([AC_CXX_ATTRIBUTE_CONST],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_REQUIRE([FANG_STD_HEADERS_ANALLY_STRICT])
AC_CACHE_CHECK([whether compiler accepts __attribute__((const))],
[ac_cv_cxx_attribute_const],
[AC_LANG_PUSH(C++)
	saved_CXXFLAGS=$CXXFLAGS
	dnl CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
	CXXFLAGS="$CONFTEST_CXXFLAGS"
	AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([#include <cstdlib>
		void pure_func (void) __attribute__ ((const));
		void pure_func (void) { exit (1); }
		],[pure_func();]),
		[ac_cv_cxx_attribute_const=yes],
		[ac_cv_cxx_attribute_const=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
dnl AC_MSG_RESULT([$ac_cv_cxx_attribute_const])
if test "$ac_cv_cxx_attribute_const" = "yes"; then
AC_DEFINE(HAVE_ATTRIBUTE_CONST, [],
	[True if compiler supports __attribute__((const)) ])
fi
])

dnl
dnl Check for __attribute__ ((pure))
dnl
AC_DEFUN([AC_CXX_ATTRIBUTE_PURE],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_REQUIRE([FANG_STD_HEADERS_ANALLY_STRICT])
AC_CACHE_CHECK([whether compiler accepts __attribute__((pure))],
[ac_cv_cxx_attribute_pure],
[AC_LANG_PUSH(C++)
	saved_CXXFLAGS=$CXXFLAGS
	dnl CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
	CXXFLAGS="$CONFTEST_CXXFLAGS"
	AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([#include <cstdlib>
		void pure_func (void) __attribute__ ((pure));
		void pure_func (void) { exit (1); }
		],[pure_func();]),
		[ac_cv_cxx_attribute_pure=yes],
		[ac_cv_cxx_attribute_pure=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
dnl AC_MSG_RESULT([$ac_cv_cxx_attribute_pure])
if test "$ac_cv_cxx_attribute_pure" = "yes"; then
AC_DEFINE(HAVE_ATTRIBUTE_PURE, [],
	[True if compiler supports __attribute__((pure)) ])
fi
])

dnl
dnl Check for __attribute__ ((holy))
dnl
AC_DEFUN([AC_CXX_ATTRIBUTE_HOLY],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_REQUIRE([FANG_STD_HEADERS_ANALLY_STRICT])
AC_CACHE_CHECK([whether compiler accepts __attribute__((holy))],
[ac_cv_cxx_attribute_holy],
[AC_LANG_PUSH(C++)
	saved_CXXFLAGS=$CXXFLAGS
	dnl CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
	CXXFLAGS="$CONFTEST_CXXFLAGS"
	AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([#include <cstdlib>
		void holy_func (void) __attribute__ ((holy));
		void holy_func (void) { exit (1); }
		],[holy_func();]),
		[ac_cv_cxx_attribute_holy=yes],
		[ac_cv_cxx_attribute_holy=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
dnl AC_MSG_RESULT([$ac_cv_cxx_attribute_holy])
if test "$ac_cv_cxx_attribute_holy" = "yes"; then
AC_DEFINE(HAVE_ATTRIBUTE_HOLY, [],
	[True if compiler supports __attribute__((holy)) ])
fi
])

dnl
dnl Check for __attribute__ ((precious))
dnl
AC_DEFUN([AC_CXX_ATTRIBUTE_PRECIOUS],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_REQUIRE([FANG_STD_HEADERS_ANALLY_STRICT])
AC_CACHE_CHECK([whether compiler accepts __attribute__((precious))],
[ac_cv_cxx_attribute_precious],
[AC_LANG_PUSH(C++)
	saved_CXXFLAGS=$CXXFLAGS
	dnl CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
	CXXFLAGS="$CONFTEST_CXXFLAGS"
	AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([#include <cstdlib>
		void precious_func (void) __attribute__ ((precious));
		void precious_func (void) { exit (1); }
		],[precious_func();]),
		[ac_cv_cxx_attribute_precious=yes],
		[ac_cv_cxx_attribute_precious=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
dnl AC_MSG_RESULT([$ac_cv_cxx_attribute_precious])
if test "$ac_cv_cxx_attribute_precious" = "yes"; then
AC_DEFINE(HAVE_ATTRIBUTE_PRECIOUS, [],
	[True if compiler supports __attribute__((precious)) ])
fi
])

dnl
dnl Check for __attribute__ ((evil))
dnl
AC_DEFUN([AC_CXX_ATTRIBUTE_EVIL],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_REQUIRE([FANG_STD_HEADERS_ANALLY_STRICT])
AC_CACHE_CHECK([whether compiler accepts __attribute__((evil))],
[ac_cv_cxx_attribute_evil],
[AC_LANG_PUSH(C++)
	saved_CXXFLAGS=$CXXFLAGS
	dnl CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
	CXXFLAGS="$CONFTEST_CXXFLAGS"
	AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([#include <cstdlib>
		void evil_func (void) __attribute__ ((evil));
		void evil_func (void) { exit (1); }
		],[evil_func();]),
		[ac_cv_cxx_attribute_evil=yes],
		[ac_cv_cxx_attribute_evil=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
dnl AC_MSG_RESULT([$ac_cv_cxx_attribute_evil])
if test "$ac_cv_cxx_attribute_evil" = "yes"; then
AC_DEFINE(HAVE_ATTRIBUTE_EVIL, [],
	[True if compiler supports __attribute__((evil)) ])
fi
])

dnl
dnl Checks for __attribute__ ((noreturn))
dnl
AC_DEFUN([AC_CXX_ATTRIBUTE_NORETURN],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_REQUIRE([FANG_STD_HEADERS_ANALLY_STRICT])
AC_CACHE_CHECK([whether compiler accepts __attribute__((noreturn))],
[ac_cv_cxx_attribute_noreturn],
[AC_LANG_PUSH(C++)
	saved_CXXFLAGS=$CXXFLAGS
	dnl CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
	CXXFLAGS="$CONFTEST_CXXFLAGS"
	AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([#include <cstdlib>
		void die (int) __attribute__ ((noreturn));
		void die (int k) { exit (k); }],
		[die(1);]),
		[ac_cv_cxx_attribute_noreturn=yes],
		[ac_cv_cxx_attribute_noreturn=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
dnl AC_MSG_RESULT([$ac_cv_cxx_attribute_noreturn])
if test "$ac_cv_cxx_attribute_noreturn" = "yes"; then
AC_DEFINE(HAVE_ATTRIBUTE_NORETURN, [],
	[True if compiler supports __attribute__((noreturn)) ])
fi
])

dnl
dnl Checks for __attribute__ ((malloc))
dnl
AC_DEFUN([AC_CXX_ATTRIBUTE_MALLOC],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_CACHE_CHECK([whether compiler accepts __attribute__((malloc))],
[ac_cv_cxx_attribute_malloc],
[AC_LANG_PUSH(C++)
	saved_CXXFLAGS=$CXXFLAGS
	CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
	AC_COMPILE_IFELSE(
	AC_LANG_PROGRAM([
		void* __fangmalloc (unsigned int) __attribute__ ((malloc));
		void __fangfree(void*);],
		[void* mem = __fangmalloc(4); __fangfree(mem);]),
		[ac_cv_cxx_attribute_malloc=yes],
		[ac_cv_cxx_attribute_malloc=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
dnl AC_MSG_RESULT([$ac_cv_cxx_attribute_malloc])
if test "$ac_cv_cxx_attribute_malloc" = "yes"; then
AC_DEFINE(HAVE_ATTRIBUTE_MALLOC, [],
	[True if compiler supports __attribute__((malloc)) ])
fi
])

dnl
dnl Checking for visibility attributes, "hidden" and "default"
dnl New since gcc-4.0.
dnl
AC_DEFUN([AC_CXX_ATTRIBUTE_VISIBILITY],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_LANG_PUSH(C++)
saved_CXXFLAGS=$CXXFLAGS
CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
dnl check for __visibility__(("hidden"))
AC_CACHE_CHECK([whether compiler accepts __attribute__((visibility("hidden")))],
[ac_cv_cxx_attribute_visibility_hidden],
	AC_COMPILE_IFELSE(
		AC_LANG_PROGRAM(
		[struct __attribute__((visibility("hidden"))) foo {
			foo() { }
			~foo() { }
		};],
		[foo bar;]),
		[ac_cv_cxx_attribute_visibility_hidden=yes],
		[ac_cv_cxx_attribute_visibility_hidden=no]
	)
)
if test "$ac_cv_cxx_attribute_visibility_hidden" = "yes"; then
AC_DEFINE(HAVE_ATTRIBUTE_VISIBILITY_HIDDEN, [],
        [True if compiler supports __attribute__((visibility("hidden"))) ])
fi
dnl check for __visibility__(("default"))
AC_CACHE_CHECK([whether compiler accepts __attribute__((visibility("default")))],
[ac_cv_cxx_attribute_visibility_default],
	AC_COMPILE_IFELSE(
		AC_LANG_PROGRAM(
		[struct __attribute__((visibility("default"))) foo {
			foo() { }
			~foo() { }
		};],
		[foo bar;]),
		[ac_cv_cxx_attribute_visibility_default=yes],
		[ac_cv_cxx_attribute_visibility_default=no]
	)
)
if test "$ac_cv_cxx_attribute_visibility_default" = "yes"; then
AC_DEFINE(HAVE_ATTRIBUTE_VISIBILITY_DEFAULT, [],
        [True if compiler supports __attribute__((visibility("default"))) ])
fi
dnl restore flags and language
CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])


dnl
dnl Checking for packed attribute.
dnl
AC_DEFUN([AC_CXX_ATTRIBUTE_PACKED],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_CACHE_CHECK([whether compiler accepts __attribute__((packed))],
[ac_cv_cxx_attribute_packed],
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
		[ac_cv_cxx_attribute_packed=yes],
		[ac_cv_cxx_attribute_packed=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
if test "$ac_cv_cxx_attribute_packed" = "yes"; then
AC_DEFINE(HAVE_ATTRIBUTE_PACKED, [],
        [True if compiler supports __attribute__((packed)) ])
fi
])

dnl
dnl Checking for aligned attribute.
dnl
AC_DEFUN([AC_CXX_ATTRIBUTE_ALIGNED],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_CACHE_CHECK([whether compiler accepts __attribute__((aligned))],
[ac_cv_cxx_attribute_aligned],
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
		[ac_cv_cxx_attribute_aligned=yes],
		[ac_cv_cxx_attribute_aligned=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
if test "$ac_cv_cxx_attribute_aligned" = "yes"; then
AC_DEFINE(HAVE_ATTRIBUTE_ALIGNED, [],
        [True if compiler supports __attribute__((aligned)) ])
fi
])

dnl
dnl Checking for aligned (size) attribute.
dnl
AC_DEFUN([AC_CXX_ATTRIBUTE_ALIGNED_SIZE],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_CACHE_CHECK([whether compiler accepts __attribute__((aligned(size)))],
[ac_cv_cxx_attribute_aligned_size],
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
		[ac_cv_cxx_attribute_aligned_size=yes],
		[ac_cv_cxx_attribute_aligned_size=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
if test "$ac_cv_cxx_attribute_aligned_size" = "yes"; then
AC_DEFINE(HAVE_ATTRIBUTE_ALIGNED_SIZE, [],
        [True if compiler supports __attribute__((aligned(size))) ])
fi
])


dnl
dnl Checking for extension that allows binding of templates with
dnl fewer arguments to template with more parameters and sufficient
dnl trailing default parameters.  
dnl See: http://gcc.gnu.org/gcc-4.2/changes.html
dnl	which is where test case is stolen from.  
dnl gcc-4.2 is the first branch series that rejects this.  
dnl
AC_DEFUN([AC_CXX_DEFAULT_TEMPLATE_TEMPLATE_PARAMETER_BINDING],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_CACHE_CHECK(
[whether templates with default parameters can bind to template template parameters with fewer parameters],
[ac_cv_cxx_template_template_parameter_default_binding],
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
		[ac_cv_cxx_template_template_parameter_default_binding=yes],
		[ac_cv_cxx_template_template_parameter_default_binding=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
if test "$ac_cv_cxx_template_template_parameter_default_binding" = "yes"; then
AC_DEFINE(HAVE_DEFAULT_TEMPLATE_TEMPLATE_PARAMETER_BINDING, [],
        [Define if templates with default arguments can bind to template template parameters with fewer parameters.])
fi
])


dnl
dnl Checking whether or not template parameter may be named directly 
dnl as a base type.  
dnl The work around is to use the identity<> type-trait "util/type_traits.h"
dnl to create an indirect reference to the desired type.  
dnl
AC_DEFUN([AC_CXX_TEMPLATE_FORMAL_BASE_CLASS],
[AC_REQUIRE([FANG_ANAL_COMPILE_FLAGS])
AC_CACHE_CHECK(
[whether templates formal parameter may be named directly as base class],
[ac_cv_cxx_template_formal_base_class],
[AC_LANG_PUSH(C++)
	saved_CXXFLAGS=$CXXFLAGS
	CXXFLAGS="$saved_CXXFLAGS $ANAL_FLAGS"
	AC_COMPILE_IFELSE(
		AC_LANG_PROGRAM(
		[template <class C>
		struct S : public C { };
		], []),
		[ac_cv_cxx_template_formal_base_class=yes],
		[ac_cv_cxx_template_formal_base_class=no]
	)
	CXXFLAGS=$saved_CXXFLAGS
AC_LANG_POP(C++)
])
if test "$ac_cv_cxx_template_formal_base_class" = "yes"; then
AC_DEFINE(HAVE_TEMPLATE_FORMAL_BASE_CLASS, [],
        [Define if templates formal parameters may be named as base classes.])
fi
])


