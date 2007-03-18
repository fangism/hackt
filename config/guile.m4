dnl
dnl "config/guile.m4"
dnl	$Id: guile.m4,v 1.6 2007/03/18 23:00:50 fang Exp $
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
dnl now we can pss a different guile-config, e.g. guile-1.8-config
AC_ARG_WITH(guile-config,
AS_HELP_STRING(
	[--with-guile-config]
	[GNU Scheme extension language configuration (default=guile-config)]),
	[guile_config=$with_guile_config],
	[guile_config="guile-config"]
)
dnl check path for the guile-config specified by the user
AC_PATH_PROG(GUILE_CONFIG, $guile_config)
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

if test -n "$GUILE_CONFIG" ; then
if test -x "$GUILE_CONFIG" ; then
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
dnl what does the following test for?
AC_CHECK_FUNCS(scm_boot_guile, , libguile_b=no)
dnl AC_CHECK_FUNCS(scm_from_ulong)
AC_CHECK_FUNCS(scm_num2ulong)
AC_CHECK_FUNCS(gh_scm2ulong)
dnl the following are in guile-1.8, but not earlier
AC_CHECK_FUNCS(scm_is_pair)
AC_CHECK_FUNCS(scm_is_string)
dnl test "$libguile_b" = "no" ... warn...
dnl pop flags
LDFLAGS="$save_LDFLAGS"
CPPFLAGS="$save_CPPFLAGS"
AC_LANG_POP(C++)
else
	AC_MSG_WARN([[guile-config missing, disabling building with guile!]])
fi
AM_CONDITIONAL(HAVE_LIBGUILE, test "$ac_cv_func_scm_is_pair" = "yes")
test "$ac_cv_func_scm_is_pair" = "yes" ||
	AC_MSG_WARN([[guile-1.8 API missing, disabling building with guile!]])
fi
AC_SUBST(GUILE_CONFIG)
AC_SUBST(GUILE_CPPFLAGS)
AC_SUBST(GUILE_LDFLAGS)
dnl TODO: extract guile version information
])dnl

