# "misc/Makefile.inc"
# includable auto-Makefile snippet

# variables conditionally defined and provided in this Makefile
FUNPROGRAMS =
LIBMISCFUN =

if WANT_LIBMISCFUN
FUNPROGRAMS += sodoku-solver
LIBMISCFUN += misc/libmiscfun.la
TESTS += misc/sodoku-test.sh
endif

misc_libmiscfun_la_CPPFLAGS = -DNO_MAIN

sodoku_solver_LDADD = $(LIBMISCFUN)
sodoku_solver_SOURCES = \
 	misc/sodoku-main.cc

MISCFUN_CC_FILES = \
	misc/sodoku-solver.cc \
	misc/sodoku.cc

MISCFUN_H_FILES = $(MISCFUN_CC_FILES:.cc=.h)

misc_libmiscfun_la_SOURCES = \
 	$(MISCFUN_CC_FILES) \
 	$(MISCFUN_H_FILES)


EXTRA_DIST += misc/README \
	misc/sodoku-sample.txt

misc/sodoku-test.sh:
	@$(ECHO) #!$(SHELL) > $@
	@$(ECHO) sodoku-solver misc/sodoku-sample.txt >> $@
	@$(CHMOD) +x $@

CLEANFILES += misc/sodoku-test.sh

