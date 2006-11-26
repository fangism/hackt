# "dox/mk/Makefile.am"
#	$Id: Makefile.am,v 1.6 2006/11/26 04:41:09 fang Exp $

include $(top_srcdir)/Make.stddef

# no targets are really built in this directory
# this directory just includes files common to other subdirectories of dox.

# latex doesn't respect TEXINPUTS on \input files that start with
# a relative path, e.g. ../mk/maketexdepend
# Thus, we are burdened with copying this one file to the build directory
# to ensure a sucessful VPATH build of the documents.  

# mktexdep = maketexdepend.tex
# maketexdepend.{awk,tex} are now obsolete
EXTRA_DIST += maketexdepend.awk maketexdepend.tex
EXTRA_DIST += Make.latex scantexdepend.awk

AUTO_IGNORE += local.cvsignore
DISTCLEANFILES += local.cvsignore

local.cvsignore: $(srcdir)/../local.cvsignore
	$(CP) $? $@

# This doesn't quite work because VPATH building picks up the srcdir's:
# maketexdepend.tex:
#	@if test ! -f $@ ; then $(CP) $(srcdir)/$@ $@ ; fi
# Thus, we resort to using a phony target to achieve the same thing.  

# all-local: phony_mktexdep

# phony_mktexdep:
#	@if test ! -f $(mktexdep) ; then $(CP) $(srcdir)/$(mktexdep) $(mktexdep) ; fi

# distclean-local:
#	if test "$(srcdir)" != "." ; then $(RM) $(mktexdep) ; fi

include $(top_srcdir)/Make.global

