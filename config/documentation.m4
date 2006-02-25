dnl "config/documentation.m4"
dnl	$Id: documentation.m4,v 1.2 2006/02/25 04:54:58 fang Exp $
dnl Autoconf macros pertaining to package documentation.
dnl This provides macros for checking for latex and related programs
dnl that are used in building the documentation.  
dnl
dnl Since these macros may not necessarily be specific to the HACKT
dnl project, I've named them with a DOC_ prefix.  
dnl I hope these will be useful for future projects.  
dnl

dnl
dnl Checks for perl in path, and also records the path to perl in PERL_PATH.
dnl
AC_DEFUN([DOC_CHECK_PROG_PERL],
[AC_CHECK_PROG([PERL], perl, perl)
AC_PATH_PROG([PERL_PATH], perl)]
)

dnl
dnl Checks for doxygen, source documentation.  
dnl
AC_DEFUN([DOC_CHECK_PROG_DOXYGEN],
[AC_CHECK_PROG([DOXYGEN], doxygen, doxygen)
AM_CONDITIONAL(HAVE_DOXYGEN, test -n "$DOXYGEN")]
)

dnl
dnl Checks for dot, from graphviz, from AT&T.  
dnl Defines HAVE_DOT_TRUE/FALSE for automake.
dnl Defines HAVE_DOT for doxygen configuration.  
dnl
AC_DEFUN([DOC_CHECK_PROG_DOT],
[AC_CHECK_PROG([DOT], dot, dot)
AM_CONDITIONAL(HAVE_DOT, test -n "$DOT" )
dnl HAVE_DOT and DOT_PATH are used in doxygen's configuration
if test -n "$DOT"
then
	AC_SUBST(HAVE_DOT, "YES")
	AC_PATH_PROG([DOT_PATH], dot)
else
	AC_SUBST(HAVE_DOT, "NO")
fi
])

dnl
dnl Checks for our favorite typesetter, LaTeX.  
dnl Defines HAVE_LATEX_TRUE/FALSE for Makefiles.  
dnl Defines HAVE_LATEX for doxygen configuration.
dnl
AC_DEFUN([DOC_CHECK_PROG_LATEX],
[AC_CHECK_PROG([LATEX], latex, latex)
AM_CONDITIONAL(HAVE_LATEX, test -n "$LATEX" )
if test -n "$LATEX"
then AC_SUBST(HAVE_LATEX, "YES")
else AC_SUBST(HAVE_LATEX, "NO")
fi
])

dnl
dnl Checks for pdflatex, and defines HAVE_PDFLATEX_TRUE/FALSE for automake.  
dnl Defines HAVE_PDFLATEX for doxygen configuration.
dnl
AC_DEFUN([DOC_CHECK_PROG_PDFLATEX],
[AC_CHECK_PROG([PDFLATEX], pdflatex, pdflatex)
AM_CONDITIONAL(HAVE_PDFLATEX, test -n "$PDFLATEX" )
if test -n "$PDFLATEX"
then AC_SUBST(HAVE_PDFLATEX, "YES")
else AC_SUBST(HAVE_PDFLATEX, "NO")
fi
])

dnl
dnl Checks for dvips, a DVI to PS converter.  
dnl
AC_DEFUN([DOC_CHECK_PROG_DVIPS],
[AC_REQUIRE([DOC_CHECK_PROG_LATEX])
AC_CHECK_PROG([DVIPS], dvips, dvips)
AM_CONDITIONAL(HAVE_DVIPS, test -n "$DVIPS" )]
)

AC_DEFUN([DOC_CHECK_PROG_BIBTEX],
[AC_CHECK_PROG([BIBTEX], bibtex, bibtex)
AM_CONDITIONAL(HAVE_BIBTEX, test -n "$BIBTEX" )]
)

AC_DEFUN([DOC_CHECK_PROG_MAKEINDEX],
[AC_CHECK_PROG([MAKEINDEX], makeindex, makeindex)
AM_CONDITIONAL(HAVE_MAKEINDEX, test -n "$MAKEINDEX" )]
)

AC_DEFUN([DOC_CHECK_PROG_PS2PDF],
[AC_CHECK_PROG([PS2PDF], ps2pdf, ps2pdf)
AM_CONDITIONAL(HAVE_PS2PDF, test -n "$PS2PDF" )]
)

dnl
dnl Whether or not documents should be built.
dnl For the HACKT project, documents are in the dox directory.  
dnl
AC_DEFUN([DOC_ARG_ENABLE_DOCS],
[AC_REQUIRE([DOC_CHECK_PROG_LATEX])
AC_REQUIRE([DOC_CHECK_PROG_PDFLATEX])
AC_MSG_CHECKING([whether to build documents by default (all)])
AC_ARG_ENABLE(enable_docs,
	AC_HELP_STRING([--disable-docs],
		[Suppress default building of PS and PDF documents.]),
dnl if given
	[AM_CONDITIONAL(ENABLE_DOCS, test "$enable_docs" = "yes")
	if test "$enable_docs" = "yes"
	then    AC_MSG_RESULT([yes])
	else    AC_MSG_RESULT([no])
	fi
	],
dnl if not give
	[AM_CONDITIONAL(ENABLE_DOCS, test "yes")
	AC_MSG_RESULT([yes (default)])]
)
if test x"$enable_docs" != "xno"
then
	if test -z "$LATEX"
	then    AC_MSG_WARN([
		Want to build PS documents, but latex not found.
		Configure with --disable-docs to suppress PS builds.])
	fi
	if test -z "$PDFLATEX"
	then    AC_MSG_WARN([
		Want to build PDF documents, but pdflatex not found.
		Configure with --disable-docs to suppress PDF builds.])
	fi
fi
])


