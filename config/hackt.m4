dnl "config/hackt.m4"
dnl	$Id: hackt.m4,v 1.4 2006/04/27 00:14:35 fang Exp $
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
dnl Define an AM_CONDITIONAL flag for VPATH-specific options.
dnl For example, don't bother generating .cvsignore.
dnl
AC_DEFUN([HACKT_AUTO_CVSIGNORE],
[AM_CONDITIONAL(NO_VPATH, test "$srcdir" = ".")]
)

dnl
dnl Defines the set of flags we want to use for configure-testing.
dnl This includes some flags to disable warnings that otherwise break
dnl some configure-tests.
dnl This defines shell variables: CONFTEST_CFLAGS and CONFTEST_CXXFLAGS.
dnl
AC_DEFUN([FANG_CONFTEST_FLAGS],
[AC_REQUIRE([FANG_AM_FLAGS])
CONFTEST_CFLAGS="$FANG_DIALECT_FLAGS $FANG_WARN_FLAGS $FANG_WARN_CFLAGS $CONFTEST_NOWARN_FLAGS $CONFTEST_NOWARN_CFLAGS $CFLAGS"
CONFTEST_CXXFLAGS="$FANG_DIALECT_FLAGS $FANG_WARN_FLAGS $FANG_WARN_CXX_FLAGS $CONFTEST_NOWARN_FLAGS $CONFTEST_NOWARN_CXXFLAGS $CXXFLAGS"
])

dnl
dnl Chec for xdr header and library.
dnl This typically doesn't require a PATH argument.  
dnl
AC_DEFUN([HACKT_ARG_WITH_XDR],
[AC_REQUIRE([AC_PROG_CC])
AC_ARG_WITH(xdr,
[[  --with-xdr[=PATH]       eXternal Data Representation library.]])
xdr_include=
xdr_ldpath=
if test "$with_xdr" && test x"$with_xdr" != xno ; then
if test "$with_xdr" != yes ; then
	xdr_include="-I$with_xdr/include"
	xdr_ldpath="-L$with_xdr/lib"
fi
saved_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $xdr_include"
AC_CHECK_HEADERS([rpc/types.h])
dnl Unfortunately, the use of xdr.h re-requires <rpc/types.h>
dnl the final optional argument handles this case nicely
AC_CHECK_HEADERS([rpc/xdr.h], [], [], [
	#if HAVE_RPC_TYPES_H
	#include <rpc/types.h>
	#endif
	])
if test x"$ac_cv_header_rpc_xdr_h" = xyes && \
	test x"$ac_cv_header_rpc_types_h" = xyes ; then
XDR_INCLUDE="$xdr_include"
fi
CPPFLAGS="$saved_CPPFLAGS"
fi
dnl realine may need to link against xdr, without requiring headers
dnl Thus, we check for tputs unconditionally
saved_LDFLAGS="$LDFLAGS"
LDFLAGS="$LDFLAGS $xdr_ldpath"
dnl NOTE: SEARCH_LIBS automatically prepends LIBS
dnl expecting 'none required' on most systems
dnl we have to pass some library else will expand into bad for loop syntax.
AC_SEARCH_LIBS(xdr_char, [c],
	[XDR_LDPATH="$xdr_ldpath"
	XDR_LIB=""],
	[:]
	dnl [AC_MSG_WARN([xdr library requested but not found.])]
)
LDFLAGS="$saved_LDFLAGS"
dnl these variables are set only if corresponding headers/libraries are found
AC_SUBST(XDR_INCLUDE)
AC_SUBST(XDR_LDPATH)
AC_SUBST(XDR_LIB)
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
dnl saved_CPPFLAGS="$CPPFLAGS"
NCURSES_INCLUDE="$ncurses_include"
fi
CPPFLAGS="$saved_CPPFLAGS"
fi
dnl realine may need to link against ncurses, without requiring headers
dnl Thus, we check for tputs unconditionally
saved_LDFLAGS="$LDFLAGS"
LDFLAGS="$LDFLAGS $ncurses_ldpath"
dnl NOTE: SEARCH_LIBS automatically prepends LIBS
AC_SEARCH_LIBS(tputs, ncurses termcap,
	[NCURSES_LDPATH="$ncurses_ldpath"
	NCURSES_LIB="$ac_cv_search_tputs"
	dnl saved_LDFLAGS="$LDFLAGS"
	AC_DEFINE(HAVE_TERMCAP, [], [Define if have ncurses OR termcap])],
	[:]
	dnl check for ac_cv_search_tputs for the result of this check
)
LDFLAGS="$saved_LDFLAGS"
dnl these variables are set only if corresponding headers/libraries are found
AC_SUBST(NCURSES_INCLUDE)
AC_SUBST(NCURSES_LDPATH)
AC_SUBST(NCURSES_LIB)
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
		[GMP_INCLUDE="$gmp_include"
		dnl saved_CPPFLAGS="$CPPFLAGS"
		dnl now check for library, manually prepending LIBS here
		dnl but only temporarily for the sake of checking
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
			dnl saved_LDFLAGS="$LDFLAGS"
			dnl saved_LIBS="$LIBS"
			GMP_LDPATH="$gmp_ldpath"
			GMP_LIB="$gmp_lib"
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
dnl these variables are set only if corresponding headers/libraries are found
AC_SUBST(GMP_INCLUDE)
AC_SUBST(GMP_LDPATH)
AC_SUBST(GMP_LIB)
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

