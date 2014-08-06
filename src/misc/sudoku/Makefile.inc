# "misc/sudoku/Makefile.inc"
# includable auto-Makefile snippet
#	$Id: Makefile.inc,v 1.5 2007/01/27 05:51:00 fang Exp $
#	vim: ft=automake

# variables conditionally defined and provided in this Makefile

# defined for convenience, should this code ever wish to find a new home
# automake bungles this and produces multiple overriding targets in the Makefile
LIBSUDOKU_DIR = misc/sudoku

LIBSUDOKU = misc/sudoku/libsudoku.la

# misc_sudoku_libsudoku_la_CPPFLAGS =

LIBSUDOKU_CC_FILES = \
	misc/sudoku/libsudoku.cc

LIBSUDOKU_TCC_FILES = \
	misc/sudoku/cell.tcc \
	misc/sudoku/board.tcc \
	misc/sudoku/constraint.tcc \
	misc/sudoku/move.tcc \
	misc/sudoku/rules.tcc \
	misc/sudoku/solve.tcc \
	misc/sudoku/tableau.tcc \
	misc/sudoku/sudoku_IO.tcc

LIBSUDOKU_HH_FILES = \
	$(LIBSUDOKU_TCC_FILES:.tcc=.hh) \
	$(LIBSUDOKU_CC_FILES:.cc=.hh) \
	misc/sudoku/debug.hh \
	misc/sudoku/inttypes.hh \
	misc/sudoku/pivot.hh \
	misc/sudoku/cell_state.hh

# this variable name needs to be consistent with the path to sources
misc_sudoku_libsudoku_la_SOURCES = \
 	$(LIBSUDOKU_CC_FILES) \
 	$(LIBSUDOKU_TCC_FILES) \
 	$(LIBSUDOKU_HH_FILES)

EXTRA_DIST +=

CLEANFILES +=

if WANT_LIBMISCFUN
ALL_CXX_HEADER_FILES += $(LIBSUDOKU_HH_FILES)
ALL_TCC_FILES += $(LIBSUDOKU_TCC_FILES)
endif

SUBDIR_CVSIGNORE += $(srcdir)/misc/sudoku/.cvsignore

$(srcdir)/misc/sudoku/.cvsignore: .cvsignore
	$(CP) $< $@

