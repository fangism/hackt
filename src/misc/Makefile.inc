# "misc/Makefile.inc"
#	vim: ft=automake
# includable auto-Makefile snippet
#	$Id: Makefile.inc,v 1.6 2007/01/27 05:50:53 fang Exp $

# variables conditionally defined and provided in this Makefile
FUNPROGRAMS =
FUNLIBS =
LIBMISCFUN =

include $(srcdir)/misc/sudoku/Makefile.inc
include $(srcdir)/misc/guile-logo/Makefile.inc

if WANT_LIBMISCFUN
FUNPROGRAMS += sudoku-solver
FUNLIBS += misc/libmiscfun.la $(LIBSUDOKU)
LIBMISCFUN += misc/libmiscfun.la
TESTS += misc/sudoku-test.sh
endif

# misc_libmiscfun_la_CPPFLAGS =

sudoku_solver_LDADD = misc/libmiscfun.la
sudoku_solver_SOURCES = \
 	misc/sudoku-main.cc

MISCFUN_CC_FILES = \
	misc/sudoku-solver.cc \
	misc/sudoku.cc

MISCFUN_H_FILES = $(MISCFUN_CC_FILES:.cc=.hh)

misc_libmiscfun_la_SOURCES = \
 	$(MISCFUN_CC_FILES) \
 	$(MISCFUN_H_FILES)


EXTRA_DIST += misc/README \
	misc/sudoku-sample.txt

misc/sudoku-test.sh:
	@{ $(ECHO) "#!$(SHELL)" ; \
	$(ECHO) ./sudoku-solver $(srcdir)/misc/sudoku-sample.txt ;} > $@ ; \
	$(CHMOD) +x $@

CLEANFILES += misc/sudoku-test.sh

if WANT_LIBMISCFUN
ALL_CXX_HEADER_FILES += $(MISCFUN_H_FILES)
endif

SUBDIR_CVSIGNORE += $(srcdir)/misc/.cvsignore

$(srcdir)/misc/.cvsignore: $(srcdir)/.cvsignore
	$(CP) $< $@


