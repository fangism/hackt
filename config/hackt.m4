dnl "config/hackt.m4"
dnl	$Id: hackt.m4,v 1.1.2.4 2006/02/24 06:11:43 fang Exp $
dnl
dnl This file is for autoconf macros specific to HACKT.
dnl General-purpose macros should be based in other m4 files.  
dnl

dnl
dnl For this project, we prefer to keep libtool --silent by default,
dnl however, one can use this to enable verbose mode.
dnl NOTE: if desired this could be generalized to pass other flags.
dnl
AC_DEFUN([HACKT_ARG_ENABLE_LIBTOOL_VERBOSE],
[AC_REQUIRE([AC_PROG_LIBTOOL])
AC_ARG_ENABLE(libtool-verbose,
	AS_HELP_STRING([--enable-libtool-verbose],
		[Standard (non-silent) libtool invocations (default=disabled)])
	)
if test x"$enable_libtool_verbose" != "xyes" ; then
	dnl default --silent
	LIBTOOL="$LIBTOOL --silent"
fi
])

dnl
dnl Check for terminal library.
dnl
AC_DEFUN([HACKT_ARG_WITH_NCURSES],
[AC_REQUIRE([AC_PROG_CC])
AC_ARG_WITH(ncurses,
[[  --with-ncurses[=PATH]   Terminal manipulation library.]])
dnl check headers and libraries independently
dnl because headers may not be needed for compiling, while
dnl libraries might be needed for linking
ncurses_include=
ncurses_ldpath=
if test "$with_ncurses" && test x"$with_ncurses" != xno ; then
if test "$with_ncurses" != yes ; then
	ncurses_include="-I$with_ncurses/include"
	ncurses_ldpath="-L$with_ncurses/lib"
fi
saved_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $ncurses_include"
AC_CHECK_HEADERS([ncurses.h curses.h termcap.h])
if test x"$ac_cv_header_ncurses_h" = xyes || \
	test x"$ac_cv_header_curses_h" = xyes || \
	test x"$ac_cv_header_termcap_h" = xyes ; then
saved_CPPFLAGS="$CPPFLAGS"
fi
CPPFLAGS="$saved_CPPFLAGS"
fi
dnl realine may need to link against ncurses, without requiring headers
dnl Thus, we check for tputs unconditionally
saved_LDFLAGS="$LDFLAGS"
LDFLAGS="$LDFLAGS $ncurses_ldpath"
AC_SEARCH_LIBS(tputs, ncurses termcap,
	[saved_LDFLAGS="$LDFLAGS"
	AC_DEFINE(HAVE_TERMCAP, [], [Define if have ncurses OR termcap])],
	[:]
	dnl check for ac_cv_search_tputs for the result of this check
)
LDFLAGS="$saved_LDFLAGS"
])


dnl
dnl Check for GMP libraries and headers.
dnl
AC_DEFUN([HACKT_ARG_WITH_GMP],
[AC_REQUIRE([AC_PROG_CC])
AC_REQUIRE([AC_PROG_CXX])
AC_ARG_WITH(gmp,
[[  --with-gmp[=PATH]       GNU multi-precision arithmetic library]])
gmp_ldpath=
gmp_lib=
gmp_include=
if test x"$with_gmp" != x ; then
	if test "$with_gmp" != yes ; then
		gmp_ldpath="-L$with_gmp/lib"
		gmp_lib="-lgmp"
		gmp_include="-I$with_gmp/include"
	dnl else just look in the standard paths
	fi
	saved_CPPFLAGS="$CPPFLAGS"
	CPPFLAGS="$saved_CPPFLAGS $gmp_include"
dnl check for headers and libraries
AC_CHECK_HEADERS([gmp.h])
if test x"$ac_cv_header_gmp_h" = xyes ; then
AC_LANG_PUSH(C++)
AC_CHECK_HEADERS([gmpxx.h])
if test x"$ac_cv_header_gmpxx_h" = xyes ; then
	AC_COMPILE_IFELSE(
		AC_LANG_PROGRAM([[#include <gmpxx.h>
		#if __GNU_MP_VERSION < 4
		#error	Require GMP version 4 or higher.
		#endif
		]],[]),
		[saved_CPPFLAGS="$CPPFLAGS"	dnl commit CPPFLAGS
		dnl now check for library
		saved_LDFLAGS="$LDFLAGS"
		saved_LIBS="$LIBS"
		LDFLAGS="$LDFLAGS $gmp_ldpath"
		LIBS="$gmp_lib $LIBS"
dnl		AC_CHECK_LIB(gmp, __gmpz_init,
dnl			[saved_LDFLAGS="$LDFLAGS"	dnl commit LDFLAGS
dnl			saved_LIBS="$LIBS"],		dnl commit LIBS
dnl [AC_MSG_ERROR([Couldn't find libgmp, please specify the path to libgmp])]
dnl		)
		dnl prefer LINK_IFELSE to CHECK_LIB because GMP's 
		dnl function names are macro-defined to internal names.  
		AC_CACHE_CHECK([for linkable libgmp library],
		[ac_cv_lib_gmp_mpz_init],
		[AC_LANG_ASSERT(C++)
		AC_LINK_IFELSE(
			AC_LANG_PROGRAM([#include <gmpxx.h>
				],[mpz_t _z; mpz_init(_z); mpz_clear(_z);]),
			[ac_cv_lib_gmp_mpz_init=yes],
			[ac_cv_lib_gmp_mpz_init=no]
		)
		])
		if test "$ac_cv_lib_gmp_mpz_init" = yes ; then
			dnl commit LDFLAGS and LIBS
			saved_LDFLAGS="$LDFLAGS"
			saved_LIBS="$LIBS"
			dnl blank line
		else
			AC_MSG_ERROR([Couldn't find libgmp, please specify the path to libgmp])
		fi
		dnl restore (or commit) flags
		LDFLAGS="$saved_LDFLAGS"
		LIBS="$saved_LIBS"
		],
		dnl else found wrong version
		[AC_MSG_NOTICE([[Found wrong version of GMP, (need >= 4)]])]
		dnl will restore CPPFLAGS
	)
	dnl end AC_COMPILE_IFELSE
else
	AC_MSG_WARN([GMP requested, but gmpxx.h not found])
fi
AC_LANG_POP(C++)
else
	AC_MSG_WARN([GMP requested, but gmp.h not found])
fi
dnl end check for libraries and headers
	CPPFLAGS="$saved_CPPFLAGS"
fi
])


dnl testing the AC_ARG_ENABLE autoconf feature
dnl god-mode doesn't actually do anything... yet
AC_DEFUN([HACKT_ARG_ENABLE_GOD_MODE],
[AC_MSG_CHECKING([whether divine powers are bestowed upon thee])
AC_ARG_ENABLE(god_mode,
dnl help text
AS_HELP_STRING([--enable-god-mode],
	[Powers fit for only immortals [(default=disabled)]]),
dnl if given
	[if test "$enable_god_mode" = "yes"
	then    AC_MSG_RESULT([you wish])
	else    AC_MSG_RESULT([no])
	fi
	],
dnl if not given
	[AC_MSG_RESULT([no (default)])]
)])


dnl
dnl Just for fun. 
dnl enable or disable fun modules
dnl
AC_DEFUN([HACKT_ARG_ENABLE_FUN],
[AC_MSG_CHECKING([how bored we are])
AC_ARG_ENABLE(fun,
	AS_HELP_STRING([--enable-fun],
		[Miscellanous fun modules. (default=disabled)]),
	AM_CONDITIONAL(WANT_LIBMISCFUN, test x"$enable_fun" = "xyes"),
	AM_CONDITIONAL(WANT_LIBMISCFUN, test ! "yes")
)
if test x"$enable_fun" = "xyes"
then    AC_MSG_RESULT([quite])
	AC_DEFINE(WANT_TO_HAVE_FUN, 1, [Conditional inclusion of fun modules.])
else    AC_MSG_RESULT([not yet])
	AC_DEFINE(WANT_TO_HAVE_FUN, 0, [Conditional inclusion of fun modules.])
fi
])

dnl
dnl This macro doesn't really belong here, whatever...
dnl
AC_DEFUN([HACKT_ARG_ENABLE_UNIVERSAL_BINARY],
[AC_MSG_CHECKING([whether universal binaries are requested])
AC_ARG_ENABLE(fat_binary,
	AS_HELP_STRING([--enable-fat-binary],
		[Compile programs for multiple architectures.]),
	[AC_MSG_RESULT([yes, but...])
	dnl AC_CHECK_PROG(lipo)
	dnl check whether or not compiler support multiple -arch flags
	AC_MSG_NOTICE([Support for this is forthcoming for Darwin.])
	dnl if enabled, then disable dynamic dependence-tracking
	],
	[AC_MSG_RESULT([no])]
)
])

dnl
dnl Sleep is evil.
dnl
AC_DEFUN([AC_PROG_SLEEP],
[AC_CHECK_PROG([SLEEP], sleep, sleep)
if test ! -n "$SLEEP"
then
	AC_MSG_ERROR([How do you expect to work without sleep?])
fi
AC_MSG_CHECKING([for *enough* sleep])
AC_MSG_RESULT([maybe])
])

dnl
dnl Sanity check.
dnl
AC_DEFUN([HACKT_CHECK_SANITY],
[AC_REQUIRE([AC_PROG_SLEEP])
dnl not so funny...
AC_MSG_CHECKING([whether fang is sane])
dnl sleep 1; echo -n "."; sleep 1; echo -n "."; sleep 1; echo -n "." sleep 1; 
$SLEEP 3
AC_MSG_RESULT([no]);
dnl not enough sleep
])

