# "misc/sudoku/Makefile.inc"
# includable auto-Makefile snippet

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

LIBSUDOKU_H_FILES = \
	$(LIBSUDOKU_TCC_FILES:.tcc=.h) \
	$(LIBSUDOKU_CC_FILES:.cc=.h) \
	misc/sudoku/debug.h \
	misc/sudoku/inttypes.h \
	misc/sudoku/pivot.h \
	misc/sudoku/cell_state.h

# this variable name needs to be consistent with the path to sources
misc_sudoku_libsudoku_la_SOURCES = \
 	$(LIBSUDOKU_CC_FILES) \
 	$(LIBSUDOKU_TCC_FILES) \
 	$(LIBSUDOKU_H_FILES)

EXTRA_DIST +=

CLEANFILES +=

ALL_HEADER_FILES += $(LIBSUDOKU_H_FILES)
ALL_TCC_FILES += $(LIBSUDOKU_TCC_FILES)

SUBDIR_CVSIGNORE += misc/sudoku/.cvsignore

misc/sudoku/.cvsignore: .cvsignore
	$(CP) $? $@

