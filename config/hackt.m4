dnl "config/hackt.m4"
dnl	$Id: hackt.m4,v 1.18 2010/03/11 18:39:14 fang Exp $
dnl
dnl This file is for autoconf macros specific to HACKT.
dnl General-purpose macros should be based in other m4 files.  
dnl

dnl @synopsis HACKT_ARG_ENABLE_LIBTOOL_VERBOSE
dnl
dnl For this project, we prefer to keep libtool --silent by default,
dnl however, one can use this to enable verbose mode.
dnl NOTE: if desired this could be generalized to pass other flags.
dnl
dnl @category ConfigureOptions
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
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

dnl @synopsis HACKT_LIBTOOL_FLAGS
dnl
dnl Appends $(LIBTOOL_FLAGS) variable, to control verbosity.
dnl
dnl @category ConfigureOptions
dnl @version 2007-07-06
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([HACKT_ARG_VAR_LIBTOOL_FLAGS],
[AC_REQUIRE([AC_PROG_LIBTOOL])
AC_ARG_VAR(LIBTOOL_FLAGS, [Additional ./libtool flags, e.g. --silent])
LIBTOOL="$LIBTOOL "'$'"(LIBTOOL_FLAGS)"
])

dnl @synopsis HACKT_AUTO_CVSIGNORE
dnl
dnl Define an AM_CONDITIONAL AUTO_CVSIGNORE flag to build cvsignore
dnl For example, don't bother generating .cvsignore.
dnl Building cvsignores is enabled by default, but disabled if the
dnl srcdir is not writeable, e.g. during a distcheck.  
dnl If the srcdir doesn't contain CVS directories, don't bother.
dnl
dnl @category ProjectSpecific
dnl @version 2008-05-28
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([HACKT_AUTO_CVSIGNORE],
[AC_MSG_CHECKING([whether to make .cvsignores])
AC_ARG_ENABLE(auto-cvsignore, 
	AS_HELP_STRING([--disable-auto-cvsignore],
		[Suppress automatic generation of srcdir's .cvsignores]),
	[],
	[enable_auto_cvsignore=yes]
)
if test -w "$srcdir/configure.ac" ; then
  srcw="yes"
else
  srcw="no"
fi
AM_CONDITIONAL(AUTO_CVSIGNORE, 
	[test "$enable_auto_cvsignore" = yes && test "$srcw" = yes && test -d "$srcdir/CVS"])
if test -d "$srcdir/CVS" ; then
if test "$enable_auto_cvsignore" = yes ; then
	if test "$srcw" = yes ; then
		AC_MSG_RESULT([yes])
	else
		AC_MSG_RESULT([no (cannot write srcdir)])
	fi
else
	AC_MSG_RESULT([no])
fi
else
	AC_MSG_RESULT([no (no CVS/ directories found)])
fi
])dnl

dnl @synopsis FANG_CONFEST_FLAGS
dnl
dnl Defines the set of flags we want to use for configure-testing.
dnl This includes some flags to disable warnings that otherwise break
dnl some configure-tests.
dnl This defines shell variables: CONFTEST_CFLAGS and CONFTEST_CXXFLAGS.
dnl
dnl @category ConfigureTests
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CONFTEST_FLAGS],
[AC_REQUIRE([FANG_AM_FLAGS])
CONFTEST_CFLAGS="$FANG_DIALECT_FLAGS $FANG_WARN_FLAGS $FANG_WARN_CFLAGS $CONFTEST_NOWARN_FLAGS $CONFTEST_NOWARN_CFLAGS $CFLAGS"
CONFTEST_CXXFLAGS="$FANG_DIALECT_FLAGS $FANG_WARN_FLAGS $FANG_WARN_CXX_FLAGS $CONFTEST_NOWARN_FLAGS $CONFTEST_NOWARN_CXXFLAGS $CXXFLAGS"
AC_MSG_NOTICE("CONFTEST_CXXFLAGS = $CONFTEST_CXXFLAGS")
])

dnl @synopsis HACKT_ARG_WITH_XDR
dnl
dnl Check for xdr header and library.
dnl This typically doesn't require a PATH argument.  
dnl Defines configure substitute variables XDR_INCLUDE for include path to
dnl header, XDR_LDPATH for path to library (if needed), XDR_LIB library name.
dnl
dnl @category InstalledPackages
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
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
dnl readline may need to link against xdr, without requiring headers
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
])dnl

dnl @synopsis HACKT_ARG_WITH_NCURSES
dnl
dnl Check for terminal manipulation library.
dnl Defines NCURSES_INCLUDE for include path to headers, 
dnl NCURSES_LDPATH for path to libraries, and 
dnl NCURSES_LIB for the library itself.  
dnl
dnl @category InstalledPackages
dnl @version 2010-03-09
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
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
dnl readline may need to link against ncurses, without requiring headers
dnl Thus, we check for tputs unconditionally
saved_LDFLAGS="$LDFLAGS"
LDFLAGS="$LDFLAGS $ncurses_ldpath"
dnl NOTE: SEARCH_LIBS automatically prepends LIBS
AC_LANG_PUSH(C)
dnl autoconf's test may not automatically extern "C"
AC_SEARCH_LIBS(tputs, ncurses termcap,
	[NCURSES_LDPATH="$ncurses_ldpath"
	NCURSES_LIB="$ac_cv_search_tputs"
	dnl saved_LDFLAGS="$LDFLAGS"
	AC_DEFINE(HAVE_TERMCAP, [], [Define if have ncurses OR termcap])],
	[:]
	dnl check for ac_cv_search_tputs for the result of this check
)
AC_LANG_POP(C)
LDFLAGS="$saved_LDFLAGS"
dnl these variables are set only if corresponding headers/libraries are found
AC_SUBST(NCURSES_INCLUDE)
AC_SUBST(NCURSES_LDPATH)
AC_SUBST(NCURSES_LIB)
])dnl


dnl @synopsis HACKT_ARG_WITH_GMP
dnl
dnl Check for GMP libraries and headers.
dnl Defines GMP_INCLUDE for include path to headers, 
dnl GMP_LDPATH for path to libraries, and 
dnl GMP_LIB for the library itself.  
dnl
dnl @category InstalledPackages
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
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
])dnl

dnl @synopsis HACKT_ARG_WITH_VPI
dnl
dnl Support for Verilog HDL co-simulation through the Programming 
dnl Language Interface.
dnl Argument is the base path where include and lib can be found 
dnl (without include/).
dnl e.g. /usr/local/cad/synopsys/vcs/linux
dnl which is expected to contain subdirs include/, lib/
dnl Often times, this will be some architecture-dependent path.
dnl Also checks for vcs compiler
dnl
dnl @category ProjectSpecific
dnl @version 2007-12-17
dnl @author David Fang
dnl @license AllPermissive
dnl
AC_DEFUN([HACKT_ARG_WITH_VPI],
[AC_ARG_WITH(vpi,
[[  --with-vpi[=PATH]       Verilog HDL PLI library]])
vpi_include="-I$with_vpi/include"
vpi_ldpath="-L$with_vpi/lib"
saved_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $vpi_include"
AC_CHECK_HEADERS(vpi_user.h)
CPPFLAGS="$saved_CPPFLAGS"
if test x"$ac_cv_header_vpi_user_h" = "xyes"
then
	VPI_INCLUDE="$vpi_include"
	VPI_LDPATH="$vpi_ldpath"
fi
AM_CONDITIONAL(HAVE_VPI, test "$VPI_INCLUDE")
AC_SUBST(VPI_INCLUDE)
AC_SUBST(VPI_LDPATH)
dnl check for vcs for running tests
AC_CHECK_PROG([VCS], vcs, vcs)
AM_CONDITIONAL(HAVE_VCS, test "$ac_cv_prog_VCS" = "vcs")
])dnl

dnl @synopsis HACKT_ARG_ENABLE_OBJECT_ALIGN_CHECKING
dnl
dnl Whether or not serialized objects should be padded with additional words
dnl for object alignment checking.  
dnl When enabled, small object increase dramatically in size.  
dnl
dnl @category ProjectSpecific
dnl @version 2006-10-23
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([HACKT_ARG_ENABLE_OBJECT_ALIGN_CHECKING],
[AC_MSG_CHECKING([whether to pad serialized objects with alignment markers])
AC_ARG_ENABLE(object_align_checking,
AS_HELP_STRING([--disable-object-align-checking],
	[Removes binary alignment markers around every serialized object [(default=enabled)]]),
[if test "$enable_object_align_checking" = "yes"
then	AC_MSG_RESULT([yes])
else	AC_MSG_RESULT([no])
fi
],
[AC_MSG_RESULT([yes (default)])
enable_object_align_checking="yes"
])
AM_CONDITIONAL(OBJECT_ALIGN_CHECKING, test "$enable_object_align_checking" = "yes")
if test "$enable_object_align_checking" = "yes"
then
AC_DEFINE(ENABLE_OBJECT_ALIGN_CHECKING, 1,
[Define to 1 to book-end every serialized object with alignment markers.])
else
AC_DEFINE(ENABLE_OBJECT_ALIGN_CHECKING, 0,
[Define to 1 to book-end every serialized object with alignment markers.])
fi
])dnl

dnl @synopsis HACKT_ARG_EMABLE_GOD_MODE
dnl
dnl Testing the AC_ARG_ENABLE autoconf feature
dnl god-mode doesn't actually do anything... yet
dnl
dnl @category ProjectSpecific
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
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
)])dnl


dnl @synopsis HACKT_ARG_ENABLE_UNROLL_PHASE
dnl
dnl Deprecated, and no longer used.
dnl Phase out unroll compile phase.
dnl Disables unroll tests.  
dnl Eventually perm this.
dnl
dnl @category ProjectSpecific
dnl @version 2006-11-28
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([HACKT_ARG_ENABLE_UNROLL_PHASE],
[AC_MSG_ERROR([This macro is obsolete, do not call it.])
AC_MSG_CHECKING([whether or not to enable unroll phase])
AC_ARG_ENABLE(unroll_phase,
	AS_HELP_STRING([--enable-unroll-phase],
		[Use (deprecated) unroll compile phase. (default=disabled)])
)
AM_CONDITIONAL(USE_UNROLL_PHASE, test x"$enable_unroll_phase" = "xyes")
if test x"$enable_unroll_phase" = "xyes"
then    AC_MSG_RESULT([yes])
	AC_DEFINE(UNIFY_UNROLL_CREATE, 0, [Define to 1 to combine unroll/create.])
else    AC_MSG_RESULT([no])
	AC_DEFINE(UNIFY_UNROLL_CREATE, 1, [Define to 1 to combine unroll/create.])
fi
])dnl


dnl @synopsis HACKT_ARG_ENABLE_FUN
dnl
dnl Just for fun. 
dnl enable or disable fun modules
dnl
dnl @category ProjectSpecific
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
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
])dnl

dnl @synopsis HACKT_ARG_ENABLE_VERBOSE_CHECK
dnl
dnl Enable to see more output dumped to stdout during make check.  
dnl
dnl @category ProjectSpecific
dnl @version 2006-11-10
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([HACKT_ARG_ENABLE_VERBOSE_CHECK],
[AC_MSG_CHECKING([for requested make-check verbosity])
AC_ARG_ENABLE(verbose_check,
	AS_HELP_STRING([--enable-verbose-check],
		[Verbose make 'check'. (default=disabled)]),
	[if test "$enable_verbose_check" = "yes"
	then    AC_MSG_RESULT([yes])
	else    AC_MSG_RESULT([no])
	fi
	],
	AC_MSG_RESULT([no (default)])
)
AM_CONDITIONAL(VERBOSE_CHECK, test x"$enable_verbose_check" = "xyes")
])dnl

dnl @synopsis HACKT_ARG_ENABLE_RANDOM_TESTS
dnl
dnl Enable to run tests with nondeterminism, randomness.
dnl
dnl @category ProjectSpecific
dnl @version 2006-11-10
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([HACKT_ARG_ENABLE_RANDOM_TESTS],
[AC_MSG_CHECKING([for requested nondeterminstic tests])
AC_ARG_ENABLE(random_tests,
	AS_HELP_STRING([--enable-random-tests],
		[Tests with nondeterminism. (default=disabled)]),
	[if test "$enable_random_tests" = "yes"
	then    AC_MSG_RESULT([yes])
	else    AC_MSG_RESULT([no])
	fi
	],
	AC_MSG_RESULT([no (default)])
)
AM_CONDITIONAL(RANDOM_TESTS, test x"$enable_random_tests" = "xyes")
])dnl

dnl @synopsis HACKT_ARG_ENABLE_INTERACTIVE_TESTS
dnl
dnl Enable to run tests that try to mimic interacive effects.  
dnl
dnl @category ProjectSpecific
dnl @version 2008-11-27
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([HACKT_ARG_ENABLE_INTERACTIVE_TESTS],
[AC_MSG_CHECKING([for requested interactive tests])
AC_ARG_ENABLE(interactive_tests,
	AS_HELP_STRING([--enable-interactive-tests],
		[Tests with interactive emulation. (default=disabled)]),
	[if test "$enable_interactive_tests" = "yes"
	then    AC_MSG_RESULT([yes])
	else    AC_MSG_RESULT([no])
	fi
	],
	AC_MSG_RESULT([no (default)])
)
AM_CONDITIONAL(INTERACTIVE_TESTS, test x"$enable_interactive_tests" = "xyes")
])dnl

dnl @synopsis HACKT_ARG_ENABLE_MULTILIB
dnl
dnl This macro doesn't really belong here, whatever...
dnl TODO: complete me!  (See example in GMP package.)
dnl
dnl @category ProjectSpecific
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([HACKT_ARG_ENABLE_MULTILIB],
[AC_MSG_CHECKING([whether multi-arch binaries are requested])
AC_ARG_ENABLE(multilib,
	AS_HELP_STRING([--enable-multilib],
		[Compile programs for multiple architectures.]),
	[AC_MSG_RESULT([yes, but...])
	dnl AC_CHECK_PROG(lipo)
	dnl check whether or not compiler support multiple -arch flags
	AC_MSG_NOTICE([Support for this is forthcoming for Darwin.])
	dnl if enabled, then disable dynamic dependence-tracking
	],
	[AC_MSG_RESULT([no])]
)
])dnl

dnl @synopsis FANG_PROG_SLEEP
dnl
dnl Checks for the sleep command.  
dnl SlEEp is EEvil.
dnl
dnl @category InstalledPackages
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_PROG_SLEEP],
[AC_CHECK_PROG([SLEEP], sleep, sleep)
if test ! -n "$SLEEP"
then
	AC_MSG_ERROR([How do you expect to work without sleep?])
fi
AC_MSG_CHECKING([for *enough* sleep])
AC_MSG_RESULT([maybe])
])dnl

dnl @synopsis HACKT_CHECK_SANITY
dnl
dnl Sanity check.
dnl
dnl @category Miscellanous
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([HACKT_CHECK_SANITY],
[AC_REQUIRE([FANG_PROG_SLEEP])
dnl not so funny...
AC_MSG_CHECKING([whether fang is sane])
dnl $SLEEP 1; printf "."; $SLEEP 1; printf "."; $SLEEP 1; printf "." $SLEEP 1; 
$SLEEP 3
AC_MSG_RESULT([no]);
dnl not enough sleep
])

dnl @synopsis HACKT_ARG_ENABLE_CHECK_TESTS
dnl
dnl For maintainer use only: skip the entire test suite.
dnl Occasionally useful for quick test cycles.  
dnl
AC_DEFUN([HACKT_ARG_ENABLE_CHECK_TESTS],
[AC_ARG_ENABLE(check_tests,
	AS_HELP_STRING([--disable-check-tests],
		[Suppress make 'check', for the impatient. (default=enabled)])
)
if test x"$enable_check_tests" = "xno"
then
AC_MSG_WARN([Disabling build test suite, you better know what you're doing!])
fi
AM_CONDITIONAL(CHECK_TESTS, test x"$enable_check_tests" != "xno")
])dnl

dnl @synopsis HACKT_ARG_ENABLE_INSTALL_TESTS
dnl
dnl Disable installcheck tests
dnl
AC_DEFUN([HACKT_ARG_ENABLE_INSTALL_TESTS],
[AC_ARG_ENABLE(install_tests,
	AS_HELP_STRING([--disable-install-tests],
		[Suppress make 'installcheck'. (default=enabled)])
)
if test x"$enable_install_tests" = "xno"
then
AC_MSG_WARN([Disabling install-checks, you better know what you're doing!])
fi
AM_CONDITIONAL(INSTALL_TESTS, test x"$enable_install_tests" != "xno")
])dnl

