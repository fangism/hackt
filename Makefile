# "Makefile"
# I insist on keeping this Makefile standard-make compatible!
# I'm trying to make this as self-contained as possible to avoid 
# ugly dependencies.  

MAKE = make
SHELL = /bin/sh

CC = gcc
# to use ccache, override with: make CC="ccache gcc"
# to use distcc, can even prefix compile command with "distcc"
LD = $(CC)
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

TARGETS = artc
TARBALL = art.tar.gz

DOXYGEN_CONFIG = art.doxygen.config

.SUFFIXES: .cc .o .l .yy

.cc.o:
	$(CC) $(CFLAGS) $< -o $@


default: all

all: $(TARGETS)

ART_OBJ = y.tab.o art.yy.o art_parser.o art_parser_prs.o art_parser_hse.o \
	art_parser_chp.o art_parser_expr.o art_parser_token.o \
	art_symbol_table.o art_main.o art_utils.o art_object.o
artc: $(ART_OBJ)
	$(LD) $(LDFLAGS) $(ART_OBJ) -o $@

art.yy.cc: art.l y.tab.h
	$(LEX) $(LEXFLAGS) art.l > $@


y.tab.h y.tab.cc: art.yy
	$(YACC) $(YACCFLAGS) $?
	-mv -f y.tab.c y.tab.cc

# documentation targets
docs:
	doxygen $(DOXYGEN_CONFIG)
	(cd dox/latex; make < /dev/null )

cleanlexer:
	-rm -f *.yy.*

cleanparser:
	-rm -f y.tab.*
	-rm -f *.output

clean: cleanlexer cleanparser
	-rm -f *.o
	-rm -f *.tmp.*
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
y.tab.o: art_parser.h art_macros.h \
	art_parser_prs.h art_parser_hse.h art_parser_chp.h
art_utils.o: art_utils.h
art_symbol_table.o: art_symbol_table.h
art_object.o: art_object.h
LIST_TEMPLATE_HEADERS = art_parser.h art_parser_template_methods.h \
	list_of_ptr.h list_of_ptr_template_methods.h
art_parser.o: $(LIST_TEMPLATE_HEADERS)
art_parser_expr.o: $(LIST_TEMPLATE_HEADERS)
art_parser_token.o: art_parser.h
art_parser_prs.o: art_parser_prs.h $(LIST_TEMPLATE_HEADERS)
art_parser_hse.o: art_parser_hse.h $(LIST_TEMPLATE_HEADERS)
art_parser_chp.o: art_parser_chp.h $(LIST_TEMPLATE_HEADERS)


