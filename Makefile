# "Makefile"
# I insist on keeping this Makefile standard-make compatible!
# I'm trying to make this as self-contained as possible to avoid 
# ugly dependencies.  

# MAKE = make
SHELL = /bin/sh

CAT = cat
ECHO = echo
RM = rm -f
MV = mv -f
TAR = tar -czvf

CC = gcc
# to use ccache, override with: make CC="ccache gcc"
# to use distcc, can even prefix compile command with "distcc"
LD = $(CC)
# using gcc, because Mach ld needs some additional directives on Mac...
CFLAGS = -O3 -Wall -c -g
# -fkeep-inline-functions
# turn on -O4 later...
LDFLAGS = -lc -lstdc++
#	-lfl: don't need this for flex, because classes are self-contained
#	NOTE: -lc MUST appear before -lstdc++ on Mac OS X's gcc-3.3!!!
LEX = flex
LEXFLAGS = -t
YACC = yacc
YACCFLAGS = -d -t -v
MAKEDEPEND = $(CC) -MM $(CFLAGS)


TARGETS = artc
TARBALL = art.tar.gz

DOXYGEN_CONFIG = art.doxygen.config

.SUFFIXES: .cc .o .l .yy .d

# careful using this...
# .BEGIN:	.depend

.cc.o:
	$(CC) $(CFLAGS) $< -o $@

.cc.d:
	$(MAKEDEPEND) $< > $@

default: all

all: .depend $(TARGETS)

ART_OBJ = y.tab.o art.yy.o art_parser.o art_parser_prs.o art_parser_hse.o \
	art_parser_chp.o art_parser_expr.o art_parser_token.o \
	art_symbol_table.o art_main.o art_utils.o art_object.o
ART_DEPS = $(ART_OBJ:.o=.d)

artc: $(ART_OBJ)
	$(LD) $(LDFLAGS) $(ART_OBJ) -o $@
# gmake doesn't interpret $> correctly, otherwise this would work:
#	$(LD) $(LDFLAGS) $> -o $@

# special file used by make, but not gmake...
.depend: $(ART_DEPS)
	$(CAT) $(ART_DEPS) > $@

art.yy.cc: art.l y.tab.h
	$(LEX) $(LEXFLAGS) art.l > $@


y.tab.h y.tab.cc: art.yy
	$(YACC) $(YACCFLAGS) $?
	-$(MV) y.tab.c y.tab.cc

# documentation targets
docs:
	doxygen $(DOXYGEN_CONFIG)
	(cd dox/latex; $(MAKE) < /dev/null )

cleanlexer:
	-$(RM) *.yy.*

cleanparser:
	-$(RM) y.tab.*
	-$(RM) *.output

cleandepend:
	-$(RM) *.d

clean: cleanlexer cleanparser cleandepend
	-$(RM) *.o
	-$(RM) *.tmp.*
	-$(RM) *.core

# for now don't always clobber this, until everyone else can generate docs...
nodocs:
	-$(RM) -r dox/html
	-$(RM) -r dox/latex

clobberdepend: cleandepend
	-$(RM) .depend

clobber: clean clobberdepend
	-$(RM) $(TARGETS)

tarball: clobber
	-$(RM) $(TARBALL)
	$(TAR) $(TARBALL) ./*

# header file dependencies generated with gcc -MM, saved to .depend

# gmake needs this, whereas make doesn't: it's implicit
# but make dies when it can't find it :(
# include .depend

# screw gmake...

