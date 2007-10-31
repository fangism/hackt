dnl "config/os.m4"
dnl	$Id: os.m4,v 1.1 2007/10/31 23:16:05 fang Exp $
dnl Autoconf macros for detecting variations operating systems.
dnl This file should ideally contain as few tests as possible.
dnl

dnl @synopsis FANG_STD_FILE_DESCRIPTORS
dnl
dnl Checks for standard file descriptor names.
dnl For example, cygwin is known to be missing /dev/std{in,out,err}
dnl
AC_DEFUN([FANG_STD_FILE_DESCRIPTORS],
[
dnl cygwin's bash::test differs from /bin/test on /dev/std* fake files...
dnl see: http://www.archivum.info/gnu.bash.bug/2006-11/msg00031.html
AC_CHECK_PROG([BIN_TEST], /bin/test, /bin/test)
AC_CHECK_FILES([/dev/stdin /dev/stdout /dev/stderr])
if test "$BIN_TEST" = "/bin/test" ; then
  AC_MSG_CHECKING([whether /dev/stdin *REALLY* exists (with /bin/test) ...])
  if /bin/test -r /dev/stdin ; then
	AC_MSG_RESULT(yes)
  else
	AC_MSG_RESULT(no)
	ac_cv_file__dev_stdin=no
  fi
  AC_MSG_CHECKING([whether /dev/stdout *REALLY* exists (with /bin/test) ...])
  if /bin/test -r /dev/stdout ; then
	AC_MSG_RESULT(yes)
  else
	AC_MSG_RESULT(no)
	ac_cv_file__dev_stdout=no
  fi
  AC_MSG_CHECKING([whether /dev/stderr *REALLY* exists (with /bin/test) ...])
  if /bin/test -r /dev/stderr ; then
	AC_MSG_RESULT(yes)
  else
	AC_MSG_RESULT(no)
	ac_cv_file__dev_stderr=no
  fi
fi
dnl force the above to fail (on cygwin)

AC_CHECK_FILES([/proc/self/fd/0 /proc/self/fd/1 /proc/self/fd/2])
__dev_stdin_filename=
if test "$ac_cv_file__dev_stdin" = yes ; then
	__dev_stdin_filename=/dev/stdin
elif test "$ac_cv_file__proc_self_fd_0" ; then
	__dev_stdin_filename=/proc/self/fd/0
else
	AC_MSG_ERROR([I don't know an equivalent for /dev/stdin!])
fi

__dev_stdout_filename=
if test "$ac_cv_file__dev_stdout" = yes ; then
	__dev_stdout_filename=/dev/stdout
elif test "$ac_cv_file__proc_self_fd_1" ; then
	__dev_stdout_filename=/proc/self/fd/1
else
	AC_MSG_ERROR([I don't know an equivalent for /dev/stdout!])
fi

__dev_stderr_filename=
if test "$ac_cv_file__dev_stderr" = yes ; then
	__dev_stderr_filename=/dev/stderr
elif test "$ac_cv_file__proc_self_fd_2" ; then
	__dev_stderr_filename=/proc/self/fd/2
else
	AC_MSG_ERROR([I don't know an equivalent for /dev/stderr!])
fi

AC_DEFINE_UNQUOTED(DEV_STDIN, "$__dev_stdin_filename", 
	[Define to a suitable name for stdin.])
AC_DEFINE_UNQUOTED(DEV_STDOUT, "$__dev_stdout_filename", 
	[Define to a suitable name for stdout.])
AC_DEFINE_UNQUOTED(DEV_STDERR, "$__dev_stderr_filename", 
	[Define to a suitable name for stderr.])
])

