# "Makefile"
# I insist on keeping this Makefile BSD-make compatible!
# I'm trying to make this as self-contained as possible to avoid 
# ugly dependencies.  

SHELL = /bin/sh

# generally, command variables may be substituted with standard make arguments, 
#	e.g. make CC="distcc ccache gcc" LD=/usr/bin/ld YACC=/usr/bin/yacc
# later... let configure generate Make.in

AWK = awk
CAT = cat
CPP = cpp
DIFF = diff -B
# -B: ignore blank line diffs
ECHO = echo
GREP = grep
SED = sed
RM = rm -f
MV = mv -f
TOUCH = touch
TAR = tar -czvf

CVS = cvs
# defined with intention of self-modification...
THISMAKEFILE = Makefile

CC = gcc
LD = $(CC)
# use CDEFS to pass in preprocessor macros, such as debug flags
# using gcc, because Mach ld needs some additional directives on Mac...
#	will eventually get around to self-configuring

# other potentially anal warnings not covered by -Wall, how far can we go?
MORE_WARN = -Wcast-qual -Wpointer-arith -Wtraditional -Wwrite-strings \
	-Wstrict-prototypes -Wmissing-prototypes
# "shadow" affects y.tab.o
# -Wshadow -Woverloaded-virtual

NO_WARN = -Wno-unused
# "no-unused" affects art.yy.o

# extremely anal about warnings... report as errors!
WARN_FLAGS = -Wall $(MORE_WARN) $(NO_WARN) -Werror

CFLAGS = -O2 $(WARN_FLAGS) -g -pipe $(CDEFS)
# -fkeep-inline-functions
# turn on -O4 later...
LDFLAGS = -lc -lstdc++
#	-lfl: don't need this for flex, because classes are self-contained
#	NOTE: -lc MUST appear before -lstdc++ on darwin gcc-3.3!!!
LEX = flex
LFLAGS = -t
YACC = yacc
YFLAGS = -d -t -v
MAKEDEPEND = $(CC) -MM

ARTC = artc
TARGETS = $(ARTC)
TARBALL = art.tar.gz

DOXYGEN_CONFIG = art.doxygen.config

.SUFFIXES: .cc .o .l .yy .d

# careful using this..., only bsdmake uses .BEGIN
.BEGIN:	.depend

.cc.o:
	$(CC) $(CFLAGS) -c $< -o $@

.cc.d:
	$(MAKEDEPEND) $< > $@

default: all

force:

makeinfo:
	@$(ECHO) "###############################################################################"
	@$(ECHO) "#	MAKE = $(MAKE) $(MAKEFLAGS)"
	@$(ECHO) "#	CC = $(CC) $(CFLAGS)"
	@$(ECHO) "#	MAKEDEPEND = $(MAKEDEPEND)"
	@$(ECHO) "#	LD = $(LD) $(LDFLAGS)"
	@$(ECHO) "#	LEX = $(LEX) $(LFLAGS)"
	@$(ECHO) "#	YACC = $(YACC) $(YFLAGS)"
	@$(ECHO) "# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #"

all: makeinfo .depend $(TARGETS)

ART_OBJ = y.tab.o y.union.o art.yy.o \
	art_parser.o art_parser_prs.o art_parser_hse.o \
	art_parser_chp.o art_parser_expr.o art_parser_token.o \
	art_symbol_table.o art_utils.o art_object.o art_object_expr.o \
	art_main.o

ART_DEPS = $(ART_OBJ:.o=.d)

$(ARTC): $(ART_OBJ)
	$(LD) $(LDFLAGS) $(ART_OBJ) -o $@
# gmake doesn't interpret $> correctly, otherwise the following would work:
#	$(LD) $(LDFLAGS) $> -o $@

# self-modifying makefile alert!
# BSD make implcitly include .depend if it exists, but gmake doesn't
# so we must tell gmake... however BSD make dies trying to explicitly
# include a file that doesn't exist, even though it 
# includes it implicitly (only when it exists).  
# Therefore we only append the include directive to the end of this makefile
# after the .depend has been created.  
# Be sure to clobber it before committing by using "make commit"!
.depend: $(ART_DEPS)
	@$(ECHO) "$(CAT) *.d > $@"
	@$(CAT) $(ART_DEPS) > $@
	@if ! $(GREP) -q "^include \.depend" $(THISMAKEFILE); then \
		$(ECHO) "patching $(THISMAKEFILE) ..."; \
		$(ECHO) "include .depend" >> $(THISMAKEFILE); \
	fi

art.yy.cc: art.l y.tab.h
	$(LEX) $(LFLAGS) art.l > $@

# y.tab.cc will depend on y.output.h
y.tab.h y.tab.cc y.output y.output.h: art.yy
	$(YACC) $(YFLAGS) $?
	$(AWK) -f yacc-output-to-C.awk y.output > y.output.h
	$(MV) y.tab.c y.tab.cc

art.yy.types: art.yy
	$(CAT) $? | $(GREP) -v "#include" | $(CPP) -P | $(GREP) -v pragma | \
		$(SED) -e "/^%start/,$$$$d" -e "/%{/,/%}/d" > $@


y.union.cc: y.output art.yy.types
	$(AWK) -f yacc-union-type.awk -v yaccfile=art.yy.types \
		-v include="art_parser.h art_parser_chp.h art_parser_hse.h art_parser_prs.h" \
		-v namespace=ART::parser \
		-v type=ART::parser::node y.output > $@

# regression testing
# this file defines and accumulates TEST_SUBJECTS (without their suffixes)
include test/Make.inc

regression: makeinfo clobber regression-target regression-norebuild

regression-target:
	$(MAKE) $(MAKEFLAGS) CC="$(CC)" CDEFS="-DREGRESSION_TEST_MODE=1" all

TEST_FILTER = $(AWK) -f test/state_enum_filter.awk
TEST_REPORT = test-report.txt

# just to make sure nothing crashes
regression-preliminary: cleantests
	@$(ECHO) "Running regression preliminaries...";
	@for f in $(TEST_SUBJECTS); do \
		$(ECHO) "Trying $$f.in ..."; \
		$(ARTC) < $$f.in 2>&1 | $(CAT) > /dev/null ; \
	done 2>&1 | $(CAT) > $(TEST_REPORT); 
	@if grep -v ^Trying $(TEST_REPORT) ; then \
		$(ECHO) "Uh oh!  Something crashed..."; \
		$(ECHO) "See \"$(TEST_REPORT)\" for failed tests."; \
		exit 1; \
	fi

regression-norebuild: regression-preliminary
	$(MAKE) cleantests
	@$(ECHO) "Running regression tests...";
	@for f in $(TEST_SUBJECTS); do \
		$(ARTC) < $$f.in 2>&1 | $(CAT) > $$f.test ; \
		$(TEST_FILTER) $$f.test > $$f.test.filter ; \
		if [ -f $$f.stderr ] ; then \
			$(TEST_FILTER) $$f.stderr > $$f.stderr.filter ; \
		else \
			$(TOUCH) $$f.stderr.filter ; \
		fi; \
		$(DIFF) $$f.stderr.filter $$f.test.filter 2>&1 | $(CAT) > $$f.diff ; \
		if [ -s $$f.diff ] ; then \
			$(ECHO) "$$f.diff is non-empty!"; \
			$(ECHO) $$f.diff >> $(TEST_REPORT); \
		fi; \
	done;
	@if [ -s $(TEST_REPORT) ] ; then \
		$(ECHO) "Test output differences were found!"; \
		$(ECHO) "See \"$(TEST_REPORT)\" for list of differences."; \
		$(ECHO) "Fix them before committing! (or else...)"; \
		exit 1; \
	else \
		$(ECHO) "All tests passed."; \
	fi


# documentation targets
docs:
	doxygen $(DOXYGEN_CONFIG)
	(cd dox/latex; $(MAKE) < /dev/null )

cleanlexer:
	-$(RM) *.yy.*

cleanparser:
	-$(RM) y.tab.*
	-$(RM) y.output
	-$(RM) y.output.h
	-$(RM) y.union.cc

cleandepend:
	-$(RM) *.d

cleantests:
	@$(ECHO) "rm -rf *.test *.diff *.filter"
	-@$(RM) $(TEST_SUBJECTS:=.test)
	-@$(RM) $(TEST_SUBJECTS:=.test.filter)
	-@$(RM) $(TEST_SUBJECTS:=.stderr.filter)
	-@$(RM) $(TEST_SUBJECTS:=.diff)
	-$(RM) $(TEST_REPORT)

clean: cleanlexer cleanparser cleandepend cleantests
	-$(RM) *.o
	-$(RM) *.tmp.*
	-$(RM) *.core

# for now don't always clobber this, until everyone else can generate docs...
nodocs:
	-$(RM) -r dox/html
	-$(RM) -r dox/latex

# self-modifying makefile alert!
# removing explicit inclusion of .depend because BSD make dies when it
# doesn't exist, even though it includes it implicitly (only when it exists)
# It will be re-generated by the .depend build rule
clobberdepend: cleandepend
	-$(RM) .depend
	@$(ECHO) "cleaning $(THISMAKEFILE) ..."
	@$(GREP) -v "^include \.depend" $(THISMAKEFILE) > $(THISMAKEFILE).temp; \
	$(MV) $(THISMAKEFILE).temp $(THISMAKEFILE)

clobber: clean clobberdepend
	-$(RM) $(TARGETS)

tarball: clobber
	-$(RM) $(TARBALL)
	$(TAR) $(TARBALL) ./*

# if regression fails, make will abort and abandon commit
# strongly suggest running with ccache to speedup re-build
commit: regression
	$(MAKE) clobberdepend
	-$(MAKE) cvsdiffs
	$(CVS) commit

cvsdiffs: force
	-$(CVS) diff > $@

# header file dependencies generated with gcc -MM, saved to .depend

# gmake needs to include dependencies explicitly, 
# whereas BSD make doesn't: it's implicit
# but BSD make dies when it can't find it :(
# hence the self-modifying Makefile
# the following line will magically appear and disappear...
