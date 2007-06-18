dnl "config/documentation.m4"
dnl	$Id: documentation.m4,v 1.7 2007/06/18 18:24:29 fang Exp $
dnl Autoconf macros pertaining to package documentation.
dnl This provides macros for checking for latex and related programs
dnl that are used in building the documentation.  
dnl
dnl Since these macros may not necessarily be specific to the HACKT
dnl project, I've named them with a DOC_ prefix.  
dnl I hope these will be useful for future projects.  
dnl

dnl @synopsis DOC_CHECK_PROG_PERL
dnl
dnl Checks for perl in path, and also records the path to perl in PERL_PATH.
dnl
dnl @category InstalledPackages
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([DOC_CHECK_PROG_PERL],
[AC_CHECK_PROG([PERL], perl, perl)
AC_PATH_PROG([PERL_PATH], perl)]
)dnl

dnl @synopsis DOC_CHECK_PROG__DOXYGEN
dnl
dnl Checks for doxygen, source documentation.  
dnl Defines DOXYGEN and automake HAVE_DOXYGEN.  
dnl
dnl @category InstalledPackages
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([DOC_CHECK_PROG_DOXYGEN],
[AC_CHECK_PROG([DOXYGEN], doxygen, doxygen)
AM_CONDITIONAL(HAVE_DOXYGEN, test -n "$DOXYGEN")]
)dnl

dnl @synopsis DOC_CHECK_PROG_DOT
dnl
dnl Checks for dot, from graphviz, from AT&T.  
dnl Also checks for family of programs: neato, fdp, twopi, circo.  
dnl Defines HAVE_DOT_TRUE/FALSE for automake.
dnl Defines HAVE_DOT for doxygen configuration.  
dnl
dnl @category InstalledPackages
dnl @version 2006-07-28
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([DOC_CHECK_PROGS_GRAPHVIZ],
[AC_MSG_NOTICE([Checking for installed graphviz tools...])
AC_CHECK_PROG([DOT], dot, dot)
AM_CONDITIONAL(HAVE_DOT, test -n "$DOT" )
dnl HAVE_DOT and DOT_PATH are used in doxygen's configuration
if test -n "$DOT"
then
	AC_SUBST(HAVE_DOT, "YES")
	AC_PATH_PROG([DOT_PATH], dot)
else
	AC_SUBST(HAVE_DOT, "NO")
fi
AC_CHECK_PROG([NEATO], neato, neato)
AM_CONDITIONAL(HAVE_NEATO, test -n "$NEATO" )
AC_CHECK_PROG([CIRCO], circo, circo)
AM_CONDITIONAL(HAVE_CIRCO, test -n "$CIRCO" )
AC_CHECK_PROG([TWOPI], twopi, twopi)
AM_CONDITIONAL(HAVE_TWOPI, test -n "$TWOPI" )
AC_CHECK_PROG([FDP], fdp, fdp)
AM_CONDITIONAL(HAVE_FDP, test -n "$FDP" )
dnl interactive editors and command interpreters
AC_CHECK_PROG([DOTTY], dotty, dotty)
AM_CONDITIONAL(HAVE_DOTTY, test -n "$DOTTY" )
AC_CHECK_PROG([LEFTY], lefty, lefty)
AM_CONDITIONAL(HAVE_LEFTY, test -n "$LEFTY" )
])dnl

dnl @synopsis DOC_CHECK_PROG_LATEX
dnl
dnl Checks for our favorite typesetter, LaTeX.  
dnl Defines HAVE_LATEX_TRUE/FALSE for Makefiles.  
dnl Defines HAVE_LATEX for doxygen configuration.
dnl
dnl @category InstalledPackages
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([DOC_CHECK_PROG_LATEX],
[AC_CHECK_PROG([LATEX], latex, latex)
AM_CONDITIONAL(HAVE_LATEX, test -n "$LATEX" )
if test -n "$LATEX"
then AC_SUBST(HAVE_LATEX, "YES")
else AC_SUBST(HAVE_LATEX, "NO")
fi
])dnl

dnl @synopsis DOC_CHECK_PROG_PDFLATEX
dnl
dnl Checks for pdflatex, and defines HAVE_PDFLATEX_TRUE/FALSE for automake.  
dnl Defines HAVE_PDFLATEX for doxygen configuration.
dnl
dnl @category InstalledPackages
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([DOC_CHECK_PROG_PDFLATEX],
[AC_CHECK_PROG([PDFLATEX], pdflatex, pdflatex)
AM_CONDITIONAL(HAVE_PDFLATEX, test -n "$PDFLATEX" )
if test -n "$PDFLATEX"
then AC_SUBST(HAVE_PDFLATEX, "YES")
else AC_SUBST(HAVE_PDFLATEX, "NO")
fi
])dnl

dnl @synopsis FANG_CHECK_PROGS_TETEX
dnl
dnl Checks for bunch of LaTeX-related executables in path.  
dnl Just a wrapper to other calls.  
dnl
dnl @category InstalledPackages
dnl @version 2006-07-28
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CHECK_PROGS_TETEX],
[AC_REQUIRE([DOC_CHECK_PROG_LATEX])
AC_REQUIRE([DOC_CHECK_PROG_PDFLATEX])
AC_REQUIRE([DOC_CHECK_PROG_BIBTEX])
AC_REQUIRE([DOC_CHECK_PROG_MAKEINDEX])
])

dnl @synopsis FANG_CHECK_PROGS_XFIG
dnl
dnl Checks for xfig, transfig, and fig2dev.
dnl Defines automake variable HAVE_FIG2DEV. 
dnl
dnl @category InstalledPackages
dnl @version 2006-07-28
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([FANG_CHECK_PROGS_XFIG],
[AC_CHECK_PROG([XFIG], xfig, xfig)
AC_CHECK_PROG([TRANSFIG], transfig, transfig)
AC_CHECK_PROG([FIG2DEV], fig2dev, fig2dev)
AM_CONDITIONAL(HAVE_FIG2DEV, test -n "$FIG2DEV" )

if test -n "$FIG2DEV" ; then
AC_CACHE_CHECK([whether fig2dev accepts -Lpdftex],
[ac_cv_fig2dev_pdftex], [
cat > conftest.fig <<ACEOF
#FIG 3.2
Portrait
Center
Metric
A4
100.00
Single
-2
1200 2
ACEOF

"$FIG2DEV" -Lpdftex < conftest.fig conftest.pdf 2> conftest.err
if test $? = 0 ; then
  ac_cv_fig2dev_pdftex=yes
else
  ac_cv_fig2dev_pdftex=no
fi
rm -f conftest.fig conftest.pdf
])
fi
if test -f conftest.err ; then cat conftest.err ; fi
rm -f conftest.err
AM_CONDITIONAL(FIG2DEV_PDFTEX, test "x$ac_cv_fig2dev_pdftex" = "xyes")
])

dnl @synopsis DOC_CHECK_PROG_DVIPS
dnl
dnl Checks for dvips, a DVI to PS converter.  
dnl
dnl @category InstalledPackages
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([DOC_CHECK_PROG_DVIPS],
[AC_REQUIRE([DOC_CHECK_PROG_LATEX])
AC_CHECK_PROG([DVIPS], dvips, dvips)
AM_CONDITIONAL(HAVE_DVIPS, test -n "$DVIPS" )]
)dnl

dnl @synopsis DOC_CHECK_PROG_BIBTEX
dnl
dnl Defines BIBTEX variable and HAVE_BIBTEX for automake.  
dnl
dnl @category InstalledPackages
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([DOC_CHECK_PROG_BIBTEX],
[AC_CHECK_PROG([BIBTEX], bibtex, bibtex)
AM_CONDITIONAL(HAVE_BIBTEX, test -n "$BIBTEX" )]
)dnl

dnl @synopsis DOC_CHECK_PROG_MAKEINDEX
dnl
dnl Defines MAKEINDEX variable and HAVE_MAKEINDEX for automake.  
dnl
dnl @category InstalledPackages
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([DOC_CHECK_PROG_MAKEINDEX],
[AC_CHECK_PROG([MAKEINDEX], makeindex, makeindex)
AM_CONDITIONAL(HAVE_MAKEINDEX, test -n "$MAKEINDEX" )]
)dnl

dnl @synopsis DOC_CHECK_PROG_PS2PDF
dnl
dnl Defines PS2PDF variable and HAVE_PS2PDF for automake.  
dnl
dnl @category InstalledPackages
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([DOC_CHECK_PROG_PS2PDF],
[AC_CHECK_PROG([PS2PDF], ps2pdf, ps2pdf)
AM_CONDITIONAL(HAVE_PS2PDF, test -n "$PS2PDF" )]
)dnl

dnl @synopsis DOC_ARG_ENABLE_DOCS
dnl
dnl Whether or not documents should be built.
dnl For the HACKT project, documents are in the dox directory.  
dnl
dnl @category InstalledPackages
dnl @version 2006-05-08
dnl @author David Fang <fangism@users.sourceforge.net>
dnl @license AllPermissive
dnl
AC_DEFUN([DOC_ARG_ENABLE_DOCS],
[AC_REQUIRE([DOC_CHECK_PROG_LATEX])
AC_REQUIRE([DOC_CHECK_PROG_PDFLATEX])
AC_MSG_CHECKING([whether to build documents by default (all)])
AC_ARG_ENABLE(docs,
	AS_HELP_STRING([--disable-docs],
		[Suppress default building of PS and PDF documents.]),
dnl if given
	[AC_MSG_RESULT([$enable_docs])],
dnl if not given
	[enable_docs="yes"
	AC_MSG_RESULT([yes (default)])]
)
AM_CONDITIONAL(ENABLE_DOCS, test x"$enable_docs" = "xyes")
if test x"$enable_docs" = "xyes"
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
else
AC_MSG_NOTICE([You can still build latex documents explicitly with 'make pdf ps'])
fi
])dnl


