# "Makefile"
# I insist on keeping this Makefile standard-make compatible!
# I'm trying to make this as self-contained as possible to avoid 
# ugly dependencies.  

MAKE = make
SHELL = /bin/sh

CC = gcc
LD = $(CC)
CFLAGS = -O3 -Wall -c -g
# turn on -O4 later...
LDFLAGS = -lc -lstdc++
#	-lfl: don't need this for flex, because classes are self-contained
#	NOTE: -lc MUST appear before -lstdc++ on Mac OS X's gcc-3.3!!!
LEX = flex
LEXFLAGS = -t
YACC = yacc
YACCFLAGS = -d -t -v

TARGETS = artc
TARBALL = art.tar.gz

DOXYGEN_CONFIG = art.doxygen.config

.SUFFIXES: .c .cc .o .l .yy

.cc.o:
	$(CC) $(CFLAGS) $< -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@


default: all

all: $(TARGETS)

ART_OBJ = art.yy.o y.tab.o art_parser.o art_symbol_table.o art_main.o \
	art_utils.o
artc: $(ART_OBJ)
	$(LD) $(LDFLAGS) $(ART_OBJ) -o $@

art.yy.cc: art.l y.tab.h
	$(LEX) $(LEXFLAGS) art.l > $@


y.tab.h y.tab.cc: art.yy
	$(YACC) $(YACCFLAGS) $?
	mv y.tab.c y.tab.cc

# documentation targets
docs:
	doxygen $(DOXYGEN_CONFIG)
	(cd dox/latex; make)

clean:
	-rm -f *.o
	-rm -f *.yy.*
	-rm -f *.tmp.*
	-rm -f y.tab.*
	-rm -f *.output
	-rm -f *.core

# for now don't always clobber this, until everyone else can generate docs...
nodocs:
	-rm -rf dox/html
	-rm -rf dox/latex

clobber: clean
	-rm -f $(TARGETS)

tarball: clobber
	-rm -f $(TARBALL)
	tar -czvf $(TARBALL) ./*

# header file dependencies could be generated automatically... not bothering
art_main.o: art_parser.h list_of_ptr.h art_switches.h
art.yy.o: y.tab.h art_lex.h
y.tab.o: art_parser.h art_macros.h art_parser_prs.h art_parser_hse.h
art_utils.o: art_utils.h
art_symbol_table.o: art_symbol_table.h
art_object.o: art_object.h

