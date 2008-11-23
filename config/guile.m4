dnl
dnl "config/guile.m4"
dnl	$Id: guile.m4,v 1.16 2008/11/23 17:53:19 fang Exp $
dnl Guile-related autoconf macros


dnl
dnl Ripped from the stepmake excerpt of lilypond's stempake/aclocal.m4
dnl Need an AC_ARG_WITH(...)
dnl
dnl AC_SUBST's the following variables
dnl	GUILE_CPPFLAGS (include path to headers)
dnl	GUILE_LDFLAGS (link path to libraries)
dnl
AC_DEFUN([FANG_GUILE], 
[
dnl now we can pass a different guile-config, e.g. guile-1.8-config
dnl also accepts --without-guile-config -> --with-guile-config=no
AC_ARG_WITH(guile-config,
AS_HELP_STRING(
	[--with-guile-config],
	[GNU Scheme extension language configuration (default=guile-config)]),
	[guile_config=$with_guile_config],
	[guile_config="guile-config"]
)


dnl not used
dnl AC_ARG_WITH(guile-snarf,
dnl AS_HELP_STRING(
dnl	[--with-guile-snarf]
dnl	[guile's automatic function processing (default=guile-snarf)]),
dnl	[guile_snarf=$with_guile_snarf],
dnl	[guile_snarf="guile-snarf"]
dnl )

dnl not used
dnl AC_ARG_WITH(guile-tools,
dnl AS_HELP_STRING(
dnl	[--with-guile-tools]
dnl	[guile's tool set (default=guile-tools)]),
dnl	[guile_tools=$with_guile_tools],
dnl	[guile_tools="guile-tools"]
dnl )

if test "x$with_guile_config" != "xno" ; then
dnl check path for the guile-config specified by the user
AC_PATH_PROG(GUILE_CONFIG, $guile_config)
dnl AC_PATH_PROG(GUILE_SNARF, $guile_snarf)
dnl AC_PATH_PROG(GUILE_TOOLS, $guile_tools)
dnl lilypond's stepmake/aclocal.m4 has example of how to check
dnl for cross-compiled with target/host
dnl AC_MSG_CHECKING([for guile-config])
dnl for guile_config in $GUILE_CONFIG guile-config ; do
dnl  AC_MSG_RESULT([$guile_config])
dnl  if ! $guile_config --version > /dev/null 2>&1 ; then
dnl    AC_MSG_WARN([cannot execute $guile_config])
dnl  else
dnl    GUILE_CONFIG=$guile_config
dnl    break
dnl  fi
dnl done

GUILE_CONFIG_VERSION="none"

if test -n "$GUILE_CONFIG" ; then
if test -x "$GUILE_CONFIG" ; then
  AC_MSG_CHECKING([guile-config version])
  GUILE_CONFIG_VERSION="`$GUILE_CONFIG --version 2>&1 | grep version`"
  AC_MSG_RESULT($GUILE_CONFIG_VERSION)
  AC_MSG_CHECKING([guile compile flags])
  GUILE_CPPFLAGS="`$GUILE_CONFIG compile`"
  AC_MSG_RESULT($GUILE_CPPFLAGS)
  AC_MSG_CHECKING([guile link flags])
  GUILE_LDFLAGS="`$GUILE_CONFIG link`"
  AC_MSG_RESULT($GUILE_LDFLAGS)
fi

dnl is it required or optional?
dnl version checking?

AC_LANG_PUSH(C++)
dnl push flags
save_CPPFLAGS="$CPPFLAGS"
save_LDFLAGS="$LDFLAGS"
CPPFLAGS="$GUILE_CPPFLAGS $CPPFLAGS"
LDFLAGS="$GUILE_LDFLAGS $LDFLAGS"
AC_CHECK_HEADERS([libguile.h guile/gh.h])
dnl <guile/gh.h> is deprecated but may have some interfaces for compatibility
if test "$ac_cv_header_libguile_h" = "yes" ; then
AC_CHECK_LIB(guile, scm_boot_guile)
AC_CHECK_LIB(guile, scm_init_guile)

guile_save_CPPFLAGS="$CPPFLAGS"
dnl gmp.h (guile-1.8+) needs std::FILE
CPPFLAGS="$CPPFLAGS -include cstdio -include libguile.h"
AC_CHECK_TYPES(scm_t_bits)
AC_CHECK_TYPES(scm_bits_t)
CPPFLAGS="$guile_save_CPPFLAGS"

dnl what does the following test for?
dnl AC_CHECK_FUNCS(scm_boot_guile, , libguile_b=no)
dnl AC_CHECK_FUNCS(scm_from_ulong)
AC_CHECK_FUNCS(scm_num2ulong)
AC_CHECK_FUNCS(gh_scm2ulong) dnl from 1.6 API
dnl the following are in guile-1.8, but not earlier
AC_CHECK_FUNCS(scm_is_bool)
AC_CHECK_FUNCS(scm_to_bool)
AC_CHECK_FUNCS(scm_is_pair)
AC_CHECK_FUNCS(scm_is_string)
AC_CHECK_FUNCS(scm_from_locale_symbol)
AC_CHECK_FUNCS(scm_str2symbol)	dnl from 1.6 API
AC_CHECK_FUNCS(scm_from_char)
AC_CHECK_FUNCS(scm_to_char)
AC_CHECK_FUNCS(scm_from_short)
AC_CHECK_FUNCS(scm_to_short)
AC_CHECK_FUNCS(scm_num2short) dnl from 1.6 API
AC_CHECK_FUNCS(scm_short2num) dnl from 1.6 API
AC_CHECK_FUNCS(scm_from_int)
AC_CHECK_FUNCS(scm_to_int)
AC_CHECK_FUNCS(scm_num2int) dnl from 1.6 API
AC_CHECK_FUNCS(scm_int2num) dnl from 1.6 API
AC_CHECK_FUNCS(scm_from_long)
AC_CHECK_FUNCS(scm_to_long)
AC_CHECK_FUNCS(scm_num2long) dnl from 1.6 API
AC_CHECK_FUNCS(scm_long2num) dnl from 1.6 API
AC_CHECK_FUNCS(scm_from_double)
AC_CHECK_FUNCS(scm_to_double)
AC_CHECK_FUNCS(scm_num2float) dnl from 1.6 API
AC_CHECK_FUNCS(scm_float2num) dnl from 1.6 API
AC_CHECK_FUNCS(scm_num2double) dnl from 1.6 API
AC_CHECK_FUNCS(scm_double2num) dnl from 1.6 API
AC_CHECK_FUNCS(scm_from_long_long)
AC_CHECK_FUNCS(scm_to_long_long)
AC_CHECK_FUNCS(scm_num2long_long) dnl from 1.6 API
AC_CHECK_FUNCS(scm_long_long2num) dnl from 1.6 API
AC_CHECK_FUNCS(scm_from_locale_string)
AC_CHECK_FUNCS(scm_to_locale_string)
AC_CHECK_FUNCS(scm_makfrom0str)	dnl from 1.6 API
AC_CHECK_FUNCS(scm_assert_smob_type)
dnl test "$libguile_b" = "no" ... warn...
dnl pop flags
LDFLAGS="$save_LDFLAGS"
CPPFLAGS="$save_CPPFLAGS"
AC_LANG_POP(C++)
else
	AC_MSG_WARN([[guile-config missing, disabling guile! (try passing GUILE_CONFIG)]])
	GUILE_CONFIG_VERSION="none"
fi
fi
fi dnl test $with_guile_config

AM_CONDITIONAL(HAVE_LIBGUILE, test "$ac_cv_lib_guile_scm_boot_guile" = "yes")

dnl AM_CONDITIONAL(HAVE_LIBGUILE, test "$ac_cv_func_scm_is_pair" = "yes")
dnl if test "$ac_cv_func_scm_is_pair" != "yes" ; then
dnl	AC_MSG_WARN([[guile-1.8 API missing, disabling guile!]])
dnl	GUILE_CONFIG_VERSION="none"
dnl fi
AC_SUBST(GUILE_CONFIG)
AC_SUBST(GUILE_CONFIG_VERSION)
dnl AC_SUBST(GUILE_SNARF)
dnl AC_SUBST(GUILE_TOOLS)
AC_SUBST(GUILE_CPPFLAGS)
AC_SUBST(GUILE_LDFLAGS)
dnl TODO: extract guile version information
])dnl

