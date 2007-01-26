dnl
dnl "config/guile.m4"
dnl	$Id: guile.m4,v 1.1 2007/01/26 18:46:38 fang Exp $
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
dnl or AC_ARG_VAR GUILE_CONFIG?
dnl AC_ARG_WITH(guile,
dnl [[  --with-guile[=PATH]     GNU Scheme extension language.]])
dnl guile_include=
dnl guile_ldpath=
dnl if test "$with_guile" && test x"$with_guile" != xno ; then
dnl if test "$with_guile" != yes ; then
dnl	guile_include="-I$with_guile/include"
dnl	guile_ldpath="-L$with_guile/lib"
dnl fi
dnl fi

AC_PATH_PROG(GUILE_CONFIG, guile-config)
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
AC_SUBST(GUILE_CONFIG)

if test -x $GUILE_CONFIG ; then
  AC_MSG_CHECKING([guile compile flags])
  GUILE_CPPFLAGS="`$GUILE_CONFIG compile`"
  AC_MSG_RESULT($GUILE_CPPFLAGS)
  AC_MSG_CHECKING([guile link flags])
  GUILE_LDFLAGS="`$GUILE_CONFIG link`"
  AC_MSG_RESULT($GUILE_LDFLAGS)
fi
AC_SUBST(GUILE_CPPFLAGS)
AC_SUBST(GUILE_LDFLAGS)

dnl is it required or optional?
dnl version checking?

AC_LANG_PUSH(C++)
dnl push flags
save_CPPFLAGS="$CPPFLAGS"
save_LDFLAGS="$LDFLAGS"
CPPFLAGS="$GUILE_CPPFLAGS $CPPFLAGS"
LDFLAGS="$GUILE_LDFLAGS $LDFLAGS"
AC_CHECK_HEADERS([libguile.h])
AC_CHECK_LIB(guile, scm_boot_guile)
dnl what does the following test for?
AC_CHECK_FUNCS(scm_boot_guile, , libguile_b=no)
dnl test "$libguile_b" = "no" ... warn...
dnl pop flags
LDFLAGS="$save_LDFLAGS"
CPPFLAGS="$save_CPPFLAGS"
AC_LANG_POP(C++)

dnl TODO: extract guile version information
])dnl

