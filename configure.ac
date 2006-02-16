# "configure.ac"
# Process this file with "autoconf" to generate the "configure" script.
# (Prior to 2005/06/19, this file was "configure.in", 
#	in case one should want to trace back the CVS revision history.)
#
#	$Id: configure.ac,v 1.39 2006/02/16 02:59:43 fang Exp $

# Hint: Try running "autoscan" for suggestions on what should go in here.  
# Results of autoscan appear in "configure.scan".  

AC_PREREQ(2.59)
AC_INIT(hackt, 0.1.4-devel-20060212, fangism@users.sourceforge.net)
AC_CONFIG_AUX_DIR(config)
AM_CONFIG_HEADER(config.h)
AC_CONFIG_SRCDIR([src/main/main_funcs.cc])

# don't need this yet
# AC_CONFIG_SUBDIRS

# Propagate the make warning flags recursively
AM_INIT_AUTOMAKE([1.9 -Wall -Werror])

# Enable or disable automatic reconfiguring.  
# AM_MAINTAINER_MODE

# Disabling shared library linking may speed up beta-testing and building.  
# AC_DISABLE_SHARED

AC_CANONICAL_HOST

# ripped from ngspice's configure.in
# TODO: rip the library and header checks too, to be invoked later
# It would be a good idea to see how they handled compatiblity between the two
dnl --with-readline:  Includes GNU readline support into CLI.  Default is "no".
dnl Including readline into ngspice is a violation of GPL license. It's use
dnl is discouraged.
AC_ARG_WITH(readline,
	AS_HELP_STRING([--with-readline[=yes/no]],
		[Enable GNU readline support for CLI.  Default=no.]))

dnl --with-editline:  Includes BSD Editline support into CLI.  Default is "no".
AC_ARG_WITH(editline,
	AS_HELP_STRING([--with-editline[=yes/no]],
		[Enable BSD editline support for CLI.  Default=no.]))

dnl readline and editline cannot both be enabled
if test "$with_readline" = "yes"; then
	AC_MSG_WARN([
	This project license is not GPL-compatible to qualify for using GNU 
	readline.  Please consider linking against BSD editline instead.])
	if test "$with_editline" = "yes"; then
		AC_MSG_ERROR(Readline and editline cannot both be enabled!)
	fi
fi

AC_CHECK_PROG([SH], sh, sh)
AC_PATH_PROG(SH_PATH, sh)

# CALLMAKE is intended for shell scripts' invocations of make
# NOT intended for direct use in configured Makefiles, because
# MAKE is more appropriate and configured for automake already.
AC_CHECK_PROGS([CALLMAKE], gmake make)

# basic program checks
AC_CHECK_PROG([CAT], cat, cat)
AC_CHECK_PROG([CPP_TRADITIONAL], cpp, cpp)
AC_CHECK_PROG([CHMOD], chmod, chmod)
# CVS is not required, but will enable some handy targets
AC_CHECK_PROG([CVS], cvs, cvs)
AM_CONDITIONAL(HAVE_CVS, test -n "$CVS")
AC_CHECK_PROG([DATE], date, date)
AC_CHECK_PROG([DIFF], diff, diff)
AC_CHECK_PROG([PATCH], patch, patch)
AC_CHECK_PROG([FIND], find, find)
AC_CHECK_PROG([GREP], grep, grep)
AC_CHECK_PROG([ECHO], echo, echo)
AC_CHECK_PROG([SORT], sort, sort)
AC_CHECK_PROG([CUT], cut, cut)
AC_CHECK_PROG([PASTE], paste, paste)
AC_CHECK_PROG([HEAD], head, head)
AC_CHECK_PROG([TAIL], tail, tail)
# AC_CHECK_PROG([PWD], pwd, pwd)
#	checking for PWD doesn't work as expected...
AC_CHECK_PROG([SED], sed, sed)
AC_CHECK_PROG([TOUCH], touch, touch)
AC_CHECK_PROG([SLEEP], sleep, sleep)
if test ! -n "$SLEEP"
then
	AC_MSG_ERROR([How do you expect to work without sleep?])
fi
AC_MSG_CHECKING([for *enough* sleep])
AC_MSG_RESULT([maybe])
AC_CHECK_PROG([INDENT], indent, indent)
AC_CHECK_PROGS([M4], gm4 m4, m4)

# AC_PROG_PERL
AC_CHECK_PROG([PERL], perl, perl)
AC_PATH_PROG([PERL_PATH], perl)

# documentation support
AC_CHECK_PROG([DOXYGEN], doxygen, doxygen)
AM_CONDITIONAL(HAVE_DOXYGEN, test -n "$DOXYGEN")

AC_CHECK_PROG([DOT], dot, dot)
AM_CONDITIONAL(HAVE_DOT, test -n "$DOT" )

AC_CHECK_PROG([LATEX], latex, latex)
AM_CONDITIONAL(HAVE_LATEX, test -n "$LATEX" )

AC_CHECK_PROG([PDFLATEX], pdflatex, pdflatex)
AM_CONDITIONAL(HAVE_PDFLATEX, test -n "$PDFLATEX" )

AC_CHECK_PROG([DVIPS], dvips, dvips)
AM_CONDITIONAL(HAVE_DVIPS, test -n "$DVIPS" )

AC_CHECK_PROG([BIBTEX], bibtex, bibtex)
AM_CONDITIONAL(HAVE_BIBTEX, test -n "$BIBTEX" )

AC_CHECK_PROG([MAKEINDEX], makeindex, makeindex)
AM_CONDITIONAL(HAVE_MAKEINDEX, test -n "$MAKEINDEX" )

AC_CHECK_PROG([PS2PDF], ps2pdf, ps2pdf)
AM_CONDITIONAL(HAVE_PS2PDF, test -n "$PS2PDF" )


# comment out until time for release
# this causes too many rebuilds during development
# AC_DEFINE_UNQUOTED(HACKT_BUILD_DATE, "`$DATE`", [Define the build date.])

# This takes care of many standard C[++] compiler configuration checks.  
AC_PROG_RANLIB
AC_PROG_LIBTOOL

# Automatically update if libtool script is outdated.  
# Adds corresponding dependencies to "Makefile.in".  
AC_SUBST(LIBTOOL_DEPS)

# appends --silent option to all libtool invocations
LIBTOOL="$LIBTOOL --silent"

dnl testing the AC_ARG_ENABLE autoconf feature
AC_MSG_CHECKING([whether divine powers are bestowed upon thee])
AC_ARG_ENABLE(god_mode,
	dnl help text
	AC_HELP_STRING([--enable-god-mode],
		[Powers fit for only immortals [(default=disabled)]]),
	dnl if given
	[if test "$enable_god_mode" = "yes"
	then	AC_MSG_RESULT([you wish])
	else	AC_MSG_RESULT([no])
	fi
	],
	dnl if not given
	[AC_MSG_RESULT([no (default)])]
)

dnl enable or disable anal header tests in src/Make.hchk-tail.am
AC_MSG_CHECKING([whether header validation is desired])
AC_ARG_ENABLE(header_validation,
	dnl help text
	AC_HELP_STRING([--disable-header-validation],
		[Perform source header validation before compiling.
		Disable to speed-up release builds [(default=enabled)]]),
	dnl if given
	[AM_CONDITIONAL(ENABLE_HEADER_VALIDATION, 
		test "$enable_header_validation" = "yes")
	if test "$enable_header_validation" = "yes"
	then	AC_MSG_RESULT([yes])
	else	AC_MSG_RESULT([no])
	fi
	],
	dnl if not given
	[AM_CONDITIONAL(ENABLE_HEADER_VALIDATION, test "yes")
	AC_MSG_RESULT([yes (default)])]
)

dnl whether or not to build universal binary
AC_MSG_CHECKING([whether universal binaries are requested])
AC_ARG_ENABLE(fat_binary,
	AC_HELP_STRING([--enable-fat-binary],
		[Compile programs for multiple architectures.]),
	[AC_MSG_RESULT([yes, but...])
	dnl AC_CHECK_PROG(lipo)
	dnl check whether or not compiler support multiple -arch flags
	AC_MSG_NOTICE([Support for this is forthcoming for Darwin.])
	dnl if enabled, then disable dynamic dependence-tracking
	],
	[AC_MSG_RESULT([no])]
)


dnl whether ot not to enable build ps and pdf targets by default
AC_MSG_CHECKING([whether to build documents by default (all)])
AC_ARG_ENABLE(enable_docs, 
	AC_HELP_STRING([--disable-docs],
		[Suppress default building of PS and PDF documents.]),
	dnl if given
	[AM_CONDITIONAL(ENABLE_DOCS, test "$enable_docs" = "yes")
	if test "$enable_docs" = "yes"
	then	AC_MSG_RESULT([yes])
	else	AC_MSG_RESULT([no])
	fi
	],
	dnl if not give
	[AM_CONDITIONAL(ENABLE_DOCS, test "yes")
	AC_MSG_RESULT([yes (default)])]
)
if test "$enable_docs" != "no"
then
	if test -z "$LATEX"
	then	AC_MSG_WARN([
		Want to build PS documents, but latex not found.
		Configure with --disable-docs to suppress PS builds.])
	fi
	if test -z "$PDFLATEX"
	then	AC_MSG_WARN([
		Want to build PDF documents, but pdflatex not found.
		Configure with --disable-docs to suppress PDF builds.])
	fi
fi

dnl enable or disable fun modules
AC_MSG_CHECKING([how bored we are])
AC_ARG_ENABLE(fun, 
	AC_HELP_STRING([--enable-fun],
		[Miscellanous fun modules. (default=disabled)]),
	AM_CONDITIONAL(WANT_LIBMISCFUN, test "$enable_fun" = "yes"),
	AM_CONDITIONAL(WANT_LIBMISCFUN, test ! "yes")
)
if test "$enable_fun" = "yes"
then	AC_MSG_RESULT([quite])
	AC_DEFINE(WANT_TO_HAVE_FUN, 1, [Conditional inclusion of fun modules.])
else	AC_MSG_RESULT([not yet])
	AC_DEFINE(WANT_TO_HAVE_FUN, 0, [Conditional inclusion of fun modules.])
fi

dnl define WORDS_BIGENDIAN if is big endian
dnl not actually needed in this project yet
AC_C_BIGENDIAN

AC_PROG_CXX
dnl we need these anal flags for checking for certain charateristics
dnl TODO: what are the equivalent flags for other compilers?
ANAL_FLAGS="-W -Wall -ansi -pedantic-errors -Werror"
cat > conftest.c <<CEOF
extern int main(int, char**);
CEOF
cp conftest.c conftest.cc
if $CC $ANAL_FLAGS -c conftest.c
then :
else AC_MSG_ERROR([Your C compiler doesn't like flags: $ANAL_FLAGS])
fi
if $CXX $ANAL_FLAGS -c conftest.cc
then :
else AC_MSG_ERROR([Your C++ compiler doesn't like flags: $ANAL_FLAGS])
fi

dnl test what flags to set in AM_CFLAGS and AM_CXXFLAGS
dnl no need for -Wsequence-point, already covered by -Wall
TRY_WARN_FLAGS="-W -Wall -Wundef -Wshadow -Wno-unused-parameter"
TRY_WARN_FLAGS="$TRY_WARN_FLAGS -Wpointer-arith -Wcast-qual -Wcast-align"
TRY_WARN_FLAGS="$TRY_WARN_FLAGS -Wconversion -Werror"
TRY_WARN_CFLAGS="-Wmissing-prototypes -Wstrict-prototypes"
TRY_WARN_CFLAGS="$TRY_WARN_CFLAGS -Wbad-function-cast -Wnested-externs"
TRY_WARN_CXXFLAGS="-Wold-style-cast -Woverloaded-virtual"
TRY_DIALECT_FLAGS="-ansi -pedantic-errors"
FANG_WARN_FLAGS=""
FANG_WARN_CFLAGS=""
FANG_WARN_CXXFLAGS=""
FANG_DIALECT_FLAGS=""
for f in $TRY_WARN_FLAGS
do
	AC_MSG_CHECKING([whether C and C++ compilers accept flag $f])
	if ( $CC $f -c conftest.c && $CXX $f -c conftest.cc )
	then
		AC_MSG_RESULT([yes])
		FANG_WARN_FLAGS="$FANG_WARN_FLAGS $f"
	else
		AC_MSG_RESULT([no])
		AC_MSG_WARN([Your C or C++ compiler doesn't like flag: $f])
	fi
done
for f in $TRY_DIALECT_FLAGS
do
	AC_MSG_CHECKING([whether C and C++ compilers accept flag $f])
	if ( $CC $f -c conftest.c && $CXX $f -c conftest.cc )
	then
		AC_MSG_RESULT([yes])
		FANG_DIALECT_FLAGS="$FANG_DIALECT_FLAGS $f"
	else
		AC_MSG_RESULT([no])
		AC_MSG_WARN([Your C or C++ compiler doesn't like flag: $f])
	fi
done
for f in $TRY_WARN_CFLAGS
do
	AC_MSG_CHECKING([whether C compiler accepts flag $f])
	if $CC $f -c conftest.c
	then
		AC_MSG_RESULT([yes])
		FANG_WARN_CFLAGS="$FANG_WARN_CFLAGS $f"
	else
		AC_MSG_RESULT([no])
		AC_MSG_WARN([Your C compiler doesn't like flag: $f])
	fi
done
for f in $TRY_WARN_CXXFLAGS
do
	AC_MSG_CHECKING([whether C++ compiler accepts flag $f])
	if $CXX $f -c conftest.cc
	then
		AC_MSG_RESULT([yes])
		FANG_WARN_CXXFLAGS="$FANG_WARN_CXXFLAGS $f"
	else
		AC_MSG_RESULT([no])
		AC_MSG_WARN([Your C++ compiler doesn't like flag: $f])
	fi
done
dnl AC_MSG_NOTICE([general warning flags: $FANG_WARN_FLAGS])
dnl AC_MSG_NOTICE([general C warning flags: $FANG_WARN_CFLAGS])
dnl AC_MSG_NOTICE([general C++ warning flags: $FANG_WARN_CXXFLAGS])
dnl AC_MSG_NOTICE([general C/C++ dialect flags: $FANG_DIALECT_FLAGS])
AC_SUBST(FANG_WARN_FLAGS)
AC_SUBST(FANG_WARN_CFLAGS)
AC_SUBST(FANG_WARN_CXXFLAGS)
AC_SUBST(FANG_DIALECT_FLAGS)

CXX_VERSION=`$CXX --version | head -n 1`
AC_SUBST(CXX_VERSION)


dnl need to set LANG to C++ to check for C++ headers

dnl which set of pre-determined compiler flags to apply?
AM_CONDITIONAL(HAVE_GXX, test "$ac_cv_cxx_compiler_gnu" = "yes")
dnl TODO: add other conditionals for other common compilers
dnl XLC++, ICC, HPCC, etc...
if ( echo "$CXX_VERSION" | grep -i prerelease )
then
	AC_MSG_WARN([
	Detected prelease version of compiler.  No PRERELEASE compilers 
	are officially supported.  Use at your own risk.])
fi

AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AM_PROG_CC_C_O
AC_ISC_POSIX

dnl more compiler characteristic checks
dnl these require the anal compile flags
SAVE_CXXFLAGS=$CXXFLAGS
CXXFLAGS="$SAVE_CXXFLAGS $ANAL_FLAGS"

AC_MSG_CHECKING([whether compiler accepts __attribute__((unused))])
AC_COMPILE_IFELSE(
AC_LANG_PROGRAM([],[
const int foo __attribute__ ((unused)) = 0;]
),
dnl action on success
[AC_MSG_RESULT([yes])
AC_DEFINE(HAVE_ATTRIBUTE_UNUSED, 1,
	[True if compiler supports __attribute__((unused)) ])],
dnl action on failure
[AC_MSG_RESULT([no])
AC_DEFINE(HAVE_ATTRIBUTE_UNUSED, 0,
	[True if compiler supports __attribute__((unused)) ])
])

dnl only need the next two tests if __attribute__((unused)) is accepted
AC_MSG_CHECKING([if __attribute__((unused)) comes before construction])
AC_COMPILE_IFELSE(
AC_LANG_PROGRAM([class foo { public: foo(const int); };],[
const foo bar __attribute__ ((unused)) (13);]
),
[AC_MSG_RESULT([yes])
AC_DEFINE(HAVE_ATTRIBUTE_UNUSED_BEFORE_CTOR, 1,
	[True if __attribute__((unused)) is accepted before construction])],
[AC_MSG_RESULT([no])
AC_DEFINE(HAVE_ATTRIBUTE_UNUSED_BEFORE_CTOR, 0,
	[True if __attribute__((unused)) is accepted before construction])
])

AC_MSG_CHECKING([if __attribute__((unused)) comes after construction])
AC_COMPILE_IFELSE(
AC_LANG_PROGRAM([class foo { public: foo(const int); };],[
const foo bar (13) __attribute__ ((unused));]
),
[AC_MSG_RESULT([yes])
AC_DEFINE(HAVE_ATTRIBUTE_UNUSED_AFTER_CTOR, 1,
	[True if __attribute__((unused)) is accepted after construction])],
[AC_MSG_RESULT([no])
AC_DEFINE(HAVE_ATTRIBUTE_UNUSED_AFTER_CTOR, 0,
	[True if __attribute__((unused)) is accepted after construction])
])

AC_MSG_CHECKING([whether compiler accepts __attribute__((const))])
AC_COMPILE_IFELSE(
AC_LANG_PROGRAM(
[#include <cstdlib>
void pure_func (void) __attribute__ ((const));
void pure_func (void) { exit (1); }
],[pure_func();]
),
[AC_MSG_RESULT([yes])
AC_DEFINE(HAVE_ATTRIBUTE_CONST, 1,
	[True if compiler supports __attribute__((const)) ])],
[AC_MSG_RESULT([no])
AC_DEFINE(HAVE_ATTRIBUTE_CONST, 0,
	[True if compiler supports __attribute__((const)) ])
])

AC_MSG_CHECKING([whether compiler accepts __attribute__((noreturn))])
AC_COMPILE_IFELSE(
AC_LANG_PROGRAM(
[#include <cstdlib>
void die (int) __attribute__ ((noreturn));
void die (int k) { exit (k); }
],[die(1);]
),
[AC_MSG_RESULT([yes])
AC_DEFINE(HAVE_ATTRIBUTE_NORETURN, 1,
	[True if compiler supports __attribute__((noreturn)) ])],
[AC_MSG_RESULT([no])
AC_DEFINE(HAVE_ATTRIBUTE_NORETURN, 0,
	[True if compiler supports __attribute__((noreturn)) ])
])


AC_MSG_CHECKING(
[whether compiler accepts __attribute__((visibility("hidden")))])
AC_COMPILE_IFELSE(
AC_LANG_PROGRAM(
[struct __attribute__((visibility("hidden"))) foo { foo() { } ~foo() { } }; ],
[foo bar;]
),
[AC_MSG_RESULT([yes])
AC_DEFINE(HAVE_ATTRIBUTE_VISIBILITY_HIDDEN, 1,
	[True if compiler supports __attribute__((visibility("hidden"))) ])],
[AC_MSG_RESULT([no])
AC_DEFINE(HAVE_ATTRIBUTE_VISIBILITY_HIDDEN, 0,
	[True if compiler supports __attribute__((visibility("hidden"))) ])
])

AC_MSG_CHECKING(
[whether compiler accepts __attribute__((visibility("default")))])
AC_COMPILE_IFELSE(
AC_LANG_PROGRAM(
[struct __attribute__((visibility("default"))) foo { foo() { } ~foo() { } }; ],
[foo bar;]
),
[AC_MSG_RESULT([yes])
AC_DEFINE(HAVE_ATTRIBUTE_VISIBILITY_DEFAULT, 1,
	[True if compiler supports __attribute__((visibility("default"))) ])],
[AC_MSG_RESULT([no])
AC_DEFINE(HAVE_ATTRIBUTE_VISIBILITY_DEFAULT, 0,
	[True if compiler supports __attribute__((visibility("default"))) ])
])

dnl restore normal compile flags
CXXFLAGS=$SAVE_CXXFLAGS

# additional header checks
AC_CHECK_HEADERS([stddef.h stdlib.h string.h unistd.h getopt.h])
AC_CHECK_HEADERS(pthread.h)
AC_CHECK_HEADERS([malloc.h])
AC_CHECK_HEADERS([ctype.h])
AC_HEADER_DIRENT
AC_CHECK_HEADERS([dirent.h sys/dirent.h])
# C++ wrappers to standard C headers
AC_CHECK_HEADERS([cstddef cstdlib cstdio cstring cassert cmath cctype])
AC_CHECK_HEADERS([cerrno cfloat climits clocale csignal csetjmp cstdarg ctime])
AC_CHECK_HEADERS([cwchar cwctype])
# checking all possible locations of (some equivalent) header files
# spanning versions gcc-2.95 to 4.x
# I bet you didn't know some of these locations were ever used!
# However, you're not supposed to use anything older than gcc-3.3 anyways...

# where is that damn hash_map?
AC_CHECK_HEADERS([ext/hash_map hash_map])
# used mainly in "src/util/hash_specializations.h"
AC_CHECK_HEADERS([ext/stl_hash_fun.h ext/hash_fun.h stl_hash_fun.h])
AC_CHECK_HEADERS([ext/hash_set hash_set])
AC_CHECK_HEADERS([ext/hashtable.h tr1/hashtable hashtable.h])
AC_CHECK_HEADERS([tr1/unordered_map tr1/unordered_set])
AC_CHECK_HEADERS([ext/functional tr1/functional])
# used in "src/util/string_fwd.h"
AC_CHECK_HEADERS([bits/stringfwd.h])
AC_CHECK_HEADERS([bits/concept_check.h])
AC_CHECK_HEADERS([bits/type_traits.h tr1/type_traits tr1/type_traits_fwd.h type_traits.h])
# among these, only sstream should be used, the rest are long gone
AC_CHECK_HEADERS([sstream strstream stringstream])
# can't include <tr1/boost_shared_ptr.h> directly
AC_CHECK_HEADERS([tr1/memory])
AC_CHECK_HEADERS([tr1/tuple tr1/array])

dnl suggestions from autoscan
AC_FUNC_ALLOCA			dnl wanted by the y.tab.c generated
AC_FUNC_CLOSEDIR_VOID		dnl wanted by src/util/dirent.cc
AC_FUNC_STAT			dnl wanted by src/util/file_status.h

# compiler characteristic checks
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_C_RESTRICT
AC_TYPE_SIZE_T
AC_C_VOLATILE
AC_CHECK_TYPES([struct __sFILE])	dnl BSD, Darwin
AC_CHECK_TYPES([struct _IO_FILE])	dnl linux
AC_CHECK_TYPES([__FILE_TAG])		dnl Sun (C++)
AC_CHECK_TYPES([struct __FILE_TAG])	dnl Sun (C)
AC_CHECK_TYPES([__FILE])		dnl Sun
AC_CHECK_TYPES([ptrdiff_t])
AC_CHECK_FUNCS([sqrt])
AC_CHECK_FUNCS([getopt])
AC_CHECK_FUNCS([strsep strtok strtok_r]) dnl strsep is missing on Solaris 2.9
AC_CHECK_FUNCS([strdup])
dnl <stdio.h>
AC_CHECK_FUNCS([fgets fputs fputc fgetc fopen fclose fflush])
AC_CHECK_FUNCS([ferror feof clearerr])
AC_CHECK_FUNCS([fseek ftell rewind fgetpos fsetpos])
AC_CHECK_FUNCS([tmpfile tmpnam tempnam])
AC_CHECK_FUNCS([remove rename])
AC_CHECK_FUNCS([setbuf setbuffer setlinebuf setvbuf])
AC_FUNC_SETVBUF_REVERSED
dnl <ctype.h>
AC_CHECK_FUNCS([isascii isspace isdigit isalpha isalnum islower isupper])
AC_CHECK_FUNCS([iscntrl isgraph ishexnumber isideogram isnumber isphonogram])
AC_CHECK_FUNCS([isspecial isprint ispunct isrune isxdigit tolower toupper])

AC_CHECK_FUNCS([system])
AC_CHECK_FUNCS([getenv setenv putenv unsetenv])

# library functions
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([calloc free])
AC_MSG_CHECKING([for free food])
AC_MSG_RESULT([YES]);

# sizeof checks, useful for checking ILP model
AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(long long)
AC_CHECK_SIZEOF(float)
AC_CHECK_SIZEOF(double)
AC_CHECK_SIZEOF(long double)
AC_CHECK_SIZEOF(void*)
AC_CHECK_SIZEOF(size_t)
AC_CHECK_SIZEOF(int64_t)
AC_CHECK_SIZEOF(uint64_t)

AC_LANG_RESTORE

# Lex/Yacc support
AM_PROG_LEX
AC_PROG_YACC

AC_ARG_VAR(LEX, [lexer/scanner, such as [f]lex])
AC_ARG_VAR(YACC, [parser generator, requires LALR(1), such as yacc/bison])

# Miscellaneous
AC_PROG_LN_S

# not so funny...
AC_MSG_CHECKING([whether fang is sane])
# sleep 1; echo -n "."; sleep 1; echo -n "."; sleep 1; echo -n "." sleep 1; 
$SLEEP 3
AC_MSG_RESULT([no]);
# not enough sleep

# check whether or not bison is disguising as yacc (with bison -y)
# some parser builds in sub-directories will compile differently
# depending on which parser is used.  
AM_CONDITIONAL(HAVE_BISON, echo "$YACC" | grep -q bison)
AM_CONDITIONAL(HAVE_BYACC, echo "$YACC" | grep -q byacc)
AM_CONDITIONAL(HAVE_YACC, echo "$YACC" | grep -v byacc | grep -q yacc)

dnl TODO: check whether or not bison/yacc works on a basic file
dnl this must be consistent with the above
dnl there might be a better way to do this...
case $YACC in
	*bison* )
		dnl echo "GOT bison"
		AC_DEFINE(USING_BISON, 1, [Define to 1 if we're using bison.])
		AC_DEFINE(USING_BYACC, 0, [Define to 1 if we're using byacc.])
		AC_DEFINE(USING_YACC, 0, [Define to 1 if we're using yacc.])
		dnl Now would be a good place to check for version
		dnl since bison is known to have significant variations.  
		YACC_VERSION=`$YACC --version 2>&1 | head -n 1`
		[BISON_VERSION=`echo x$YACC_VERSION | sed 's/[^.0-9]//g'`]
		AC_MSG_NOTICE([found bison version $BISON_VERSION.])
		dnl known version checks
dnl the below code doesn't work...
dnl	if { awk 'END{ if($BISON_VERSION +0.0<=1.35) exit 1;}' </dev/null;}
dnl	then 
dnl dnl		if { awk 'END{if($BISON_VERSION +0.0<=1.28) exit 1;}' </dev/null;}
dnl dnl		then AC_MSG_ERROR([
dnl dnl	Your bison (<= 1.28) is known to fail with this project.
dnl dnl	Please upgrade to 1.875 or higher, or use traditional yacc.])
dnl dnl		else
dnl			AC_MSG_WARN([
dnl	Your bison (<= 1.35) is not officially supported by this project.  
dnl	Consider upgrading to 1.875 or higher, or use traditional yacc.])
dnl dnl		fi
dnl	fi
		;;
	*byacc* )
		dnl echo "GOT byacc"
		AC_DEFINE(USING_BISON, 0, [Define to 1 if we're using bison.])
		AC_DEFINE(USING_BYACC, 1, [Define to 1 if we're using byacc.])
		AC_DEFINE(USING_YACC, 0, [Define to 1 if we're using yacc.])
		YACC_VERSION=`which $YACC | head -n 1`
		;;
	*yacc* )
		dnl echo "GOT yacc"
		AC_DEFINE(USING_BISON, 0, [Define to 1 if we're using bison.])
		AC_DEFINE(USING_BYACC, 0, [Define to 1 if we're using byacc.])
		AC_DEFINE(USING_YACC, 1, [Define to 1 if we're using yacc.])
		dnl traditional yacc has no version flag :(
		YACC_VERSION=`which $YACC | head -n 1`
		;;
	* ) AC_MSG_ERROR([No parser-generator found.]) ;;
esac
AC_SUBST(YACC_VERSION)

# unfortunately, lex is often IDENTICAL to flex...
case $LEX in
	*flex* )
		AC_DEFINE(USING_FLEX, 1, [Define to 1 if we're using flex.])
		AC_DEFINE(USING_LEX, 0, [Define to 1 if we're using lex.])
		;;
	*lex* )
		AC_DEFINE(USING_FLEX, 0, [Define to 1 if we're using lex.])
		AC_DEFINE(USING_LEX, 1, [Define to 1 if we're using lex.])
		;;
	* ) AC_MSG_ERROR([No lexer-generator found.]) ;;
esac
LEX_VERSION=`$LEX --version | head -n 1`
AC_SUBST(LEX_VERSION)

# some substitution variables for the doxygen configuration
if test -n "$DOT"
then
	AC_SUBST(HAVE_DOT, "YES")
	AC_PATH_PROG([DOT_PATH], dot)
else
	AC_SUBST(HAVE_DOT, "NO")
fi

if test -n "$LATEX"
then AC_SUBST(HAVE_LATEX, "YES")
else AC_SUBST(HAVE_LATEX, "NO")
fi

if test -n "$PDFLATEX"
then AC_SUBST(HAVE_PDFLATEX, "YES")
else AC_SUBST(HAVE_PDFLATEX, "NO")
fi

# figure related stuff
AC_CHECK_PROG([XFIG], xfig, xfig)
AC_CHECK_PROG([TRANSFIG], transfig, transfig)
dnl only fig2dev is needed for converting xfigs
AC_CHECK_PROG([FIG2DEV], fig2dev, fig2dev)

# checking for old CAST tools for regression and backwards compatibility tests
# no big deal if they don't exist...
# NOTE: AM_CONDITIONAL is usable in generation of AC_CONFIG_FILES
#	just use @VAR_TRUE@ instead of the if ... version in automake
AC_CHECK_PROG([PRSIM], prsim, prsim)
AC_CHECK_PROG([CFLAT], cflat, cflat)
AM_CONDITIONAL(HAVE_PRSIM, test -n "$PRSIM" )
AM_CONDITIONAL(HAVE_CFLAT, test -n "$CFLAT" )

# shamelessly copied autoconf snippet from ngspige's configure.in
dnl	Option to include GNU readline support in ngspice CLI
dnl	Default: disabled.
dnl	Hope to see in the future readline replacement.
if test "$with_readline" != "yes"; then
	AC_MSG_RESULT(GNU readline disabled.)
else
	AC_MSG_RESULT(Checking for readline:)
	AC_CHECK_HEADERS([readline/readline.h readline/history.h],
		[AC_DEFINE(HAVE_GNUREADLINE,[],
			[Define if we have GNU readline])],
		[AC_MSG_ERROR(Couldn't find GNU readline headers.)])
	AC_SEARCH_LIBS(tputs,ncurses termcap,
		AC_DEFINE(HAVE_TERMCAP,[],
			[Define if we have ncurses or termcap]),
		AC_MSG_ERROR(Found neither ncurses or termcap))
	AC_CHECK_LIB(readline, readline,
		[LIBS="$LIBS -lreadline"],
		[AC_MSG_ERROR(Couldn't find readline libraries.  Try passing extra LDFLAGS.)])
fi

dnl	Option to include BSD editline support in ngspice CLI
dnl	Default: disabled.
if test "$with_editline" != "yes"; then
	AC_MSG_RESULT(BSD editline disabled.)
else
	AC_MSG_RESULT(Checking for editline:)
	AC_CHECK_HEADERS([editline/readline.h],
		[AC_DEFINE([HAVE_BSDEDITLINE],[1],
			[Define to enable BSD editline])],
		[AC_MSG_ERROR(Couldn't find BSD editline headers.)])
	AC_SEARCH_LIBS(tputs,ncurses termcap,
		AC_DEFINE(HAVE_TERMCAP,[],
			[Define if we have ncurses or termcap]),
		AC_MSG_ERROR(Found neither ncurses or termcap))
	AC_CHECK_LIB(edit, readline,
		[LIBS="$LIBS -ledit"],
		[AC_MSG_ERROR(Couldn't find editline libraries.  Try passing extra LDFLAGS.)],
		-lncurses )
fi

dnl if test "$ac_cv_header_readline_readline_h" = "yes" || \
dnl	test "$ac_cv_header_editline_readline_h" = "yes"
if test "$with_readline" = "yes" || test "$with_editline" = "yes"
then
AC_MSG_CHECKING([if readline()'s prompt argument is const char*])
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
dnl need to have warnings enabled and turned to errors here
SAVE_CXXFLAGS=$CXXFLAGS
CXXFLAGS="$SAVE_CXXFLAGS $ANAL_FLAGS"
AC_COMPILE_IFELSE(
AC_LANG_PROGRAM([
#include <cstdio>
#if defined(HAVE_GNUREADLINE)
#include <readline/readline.h>
#elif defined(HAVE_BSDEDITLINE)
#include <editline/readline.h>
#endif
],[[[const char null[] = "foo"; readline(null) ]]]),
dnl action on success
[AC_MSG_RESULT([yes])
AC_DEFINE(READLINE_PROMPT_CONST, 1,
	[True if readline's argument is const char*])],
dnl action on failure
[AC_MSG_RESULT([no (you suck!)])
AC_DEFINE(READLINE_PROMPT_CONST, 0,
	[True if readline's argument is const char*])
])
dnl restore compile flags
CXXFLAGS=$SAVE_CXXFLAGS
AC_LANG_RESTORE
fi


dnl first, create a useful variable substitution script for use in config.status
AC_CONFIG_FILES([config_subst.awk.sh:config/config_subst.awk],
[dollar=$
at=@
cat > config_subst.awk.sh <<EOF
#!$SHELL
awk -f $srcdir/config/config_subst.awk $dollar$at
EOF
chmod +x config_subst.awk.sh
])

CONFIGDATE=`$DATE`
AC_SUBST(CONFIGDATE)

AC_CONFIG_FILES([
	Makefile
	scripts/Makefile
	src/Makefile
	src/util/test/Makefile
	test/Makefile
	test/lexer/Makefile
	test/parser/Makefile
	test/parser/basic/Makefile
	test/parser/namespace/Makefile
	test/parser/param/Makefile
	test/parser/array/Makefile
	test/parser/process/Makefile
	test/parser/prs/Makefile
	test/parser/chp/Makefile
	test/parser/datatype/Makefile
	test/parser/channel/Makefile
	test/parser/template/Makefile
	test/parser/typedef/Makefile
	test/parser/connect/Makefile
	test/parser/flow/Makefile
	test/benchmarks/Makefile
	dox/Makefile
	dox/mk/Makefile
	dox/lang/Makefile
	dox/util/Makefile
	dox/objfmt/Makefile
	dox/man/Makefile
	dox/tutorial/Makefile
	lib/Makefile
	hackt.doxygen.config
])
AC_CONFIG_FILES([scripts/replace_if_changed.sh],
	[chmod +x scripts/replace_if_changed.sh])
AC_CONFIG_FILES([src/util/test/test-expect.sh],
	[chmod +x src/util/test/test-expect.sh])
AC_CONFIG_FILES([src/scripts/deps_to_dot.awk],
	[chmod +x src/scripts/deps_to_dot.awk])
AC_CONFIG_FILES([test/parse-test-expect.sh],
	[chmod +x test/parse-test-expect.sh])
AC_CONFIG_FILES([test/hackt-obj-diff.sh],
	[chmod +x test/hackt-obj-diff.sh])
AC_CONFIG_FILES([test/hackt-unroll-expect.sh],
	[chmod +x test/hackt-unroll-expect.sh])
AC_CONFIG_FILES([test/hackt-create-expect.sh],
	[chmod +x test/hackt-create-expect.sh])
AC_CONFIG_FILES([test/check-create-consistency.sh],
	[chmod +x test/check-create-consistency.sh])
AC_CONFIG_FILES([test/hackt-alloc-expect.sh],
	[chmod +x test/hackt-alloc-expect.sh])
AC_CONFIG_FILES([test/hackt-cflat-expect.sh],
	[chmod +x test/hackt-cflat-expect.sh])
AC_CONFIG_FILES([test/hackt-cflat-sprs-expect.sh],
	[chmod +x test/hackt-cflat-sprs-expect.sh])
AC_CONFIG_FILES([test/hackt-prsim-expr-alloc-expect.sh],
	[chmod +x test/hackt-prsim-expr-alloc-expect.sh])
AC_CONFIG_FILES([test/test-series.sh],
	[chmod +x test/test-series.sh])

dnl the AC_CONFIG_HEADERS macro does not perform AC_SUBST-itution
dnl so we have to fall back onto AC_CONFIG_FILES :(
dnl It wouldve been nice to have difference checking from AC_CONFIG_HEADERS...
dnl work on this nicety later...

dnl no input file needed, just referencing arbitrary existing file
dnl AC_CONFIG_FILES([configdate.h:config/macro-value.h.in],
dnl	[out=configdate.h
dnl	date=`./config_subst.awk.sh -v var=DATE config.status`
dnl	sed 's/MACRO/CONFIGDATE/g' $srcdir/config/macro-value.h.in | \
dnl		sed "s/VALUE/\"`$date`\"/g" > $out
dnl	])

AC_CONFIG_FILES([configdate.h src/buildhost.h])
AC_CONFIG_FILES([src/cxxflags.h src/cxx_version.h])
AC_CONFIG_FILES([src/lexer/lex_version.h src/parser/yacc_version.h])

AC_CONFIG_FILES([src/parser/hackt-parse.yy:src/parser/hackt-parse.yy.in],
	[out=src/parser/hackt-parse.yy
	if ( ./config_subst.awk.sh -v var=HAVE_BISON_TRUE config.status | grep -q "#" )
	then
		sed '/%pure.parser/d' $srcdir/$out.in > $out~
	else
		cp $srcdir/$out.in $out~
	fi
	scripts/replace_if_changed.sh $out~ $out
	])

AC_CONFIG_FILES([src/parser/instref-parse.yy:src/parser/instref-parse.yy.in],
	[out=src/parser/instref-parse.yy
	if ( ./config_subst.awk.sh -v var=HAVE_BISON_TRUE config.status | grep -q "#" )
	then
		sed '/%pure.parser/d' $srcdir/$out.in > $out~
	else
		cp $srcdir/$out.in $out~
	fi
	scripts/replace_if_changed.sh $out~ $out
	])

AC_CONFIG_FILES([src/util/lang/CX/cx-parse.yy:src/util/lang/CX/cx-parse.yy.in],
	[out=src/util/lang/CX/cx-parse.yy
	if ( ./config_subst.awk.sh -v var=HAVE_BISON_TRUE config.status | grep -q "#" )
	then
		sed '/%pure.parser/d' $srcdir/$out.in > $out~
	else
		cp $srcdir/$out.in $out~
	fi
	scripts/replace_if_changed.sh $out~ $out
	])
dnl initialize and reset header check dependencies
dnl also depend on Makefile?
dnl can't run make until depfiles has run! doh... work on this later...
dnl
dnl AC_CONFIG_FILES([src/headers_deps.make],
dnl 	[out=src/headers_deps.make
dnl 	(cd src && make echo-header-depbases) | \
dnl 		awk -f $srcdir/src/scripts/automake_include.awk >> $out
dnl 	])

AC_OUTPUT

