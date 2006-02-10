# point these to hackt
top_srcdir = ../..
top_builddir = ../..
srcdir = .

# suffixes used by all test directories' Makefiles
# automake will convert these into .SUFFIXES:
.SUFFIXES: .hac .hacktcmpltest .hacktobjtest .hacktunrolltest \
	.hacktcreatetest .hacktalloctest .hacktcflattest \
	.haco .haco-u .haco-c .haco-a .prs .sprs .prsimexpr .prsimexpr-dot \
	.prsimexpr-ps

HACKT_EXE = $(top_builddir)/src/hackt
PARSE_TEST_EXE = $(HACKT_EXE) parse_test
HACKT_COMPILE_EXE = $(HACKT_EXE) compile
HACKT_OBJDUMP_EXE = $(HACKT_EXE) objdump
HACKT_UNROLL_EXE = $(HACKT_EXE) unroll
HACKT_CREATE_EXE = $(HACKT_EXE) create
HACKT_ALLOC_EXE = $(HACKT_EXE) alloc
HACKT_CFLAT_EXE = $(HACKT_EXE) cflat
HACKT_CFLAT_PRSIM_EXE = $(HACKT_CFLAT_EXE) prsim
HACKT_PRSIM_EXE = $(HACKT_EXE) prsim

.hac.haco:
	$(HACKT_COMPILE_EXE) -I$(srcdir) $< $@

.haco.haco-u:
	$(HACKT_UNROLL_EXE) $< $@

.haco-u.haco-c:
	$(HACKT_CREATE_EXE) $< $@

.haco-c.haco-a:
	$(HACKT_ALLOC_EXE) $< $@

.haco-a.prs:
	$(HACKT_CFLAT_PRSIM_EXE) $< > $@

.haco-a.sprs:
	$(HACKT_CFLAT_PRSIM_EXE) -fsizes $< > $@

.haco-a.prsimexpr:
	$(HACKT_PRSIM_EXE) -fno-run -fdump-expr-alloc $< > $@

all:

clean: clean-local

clean-local:
	-$(RM) *.core core.*
	-$(RM) *.noindex
	-$(RM) *.filter *.diff *.sort *.noindex
	-$(RM) *.indump *.outdump *.objdiff
	-$(RM) *.haco
	-$(RM) *.haco-u *.unrolldump *.unrolldiff
	-$(RM) *.haco-c *.createdump *.creatediff
	-$(RM) *.haco-a *.allocdump *.allocdiff
	-$(RM) *.prs *.prsdiff
	-$(RM) *.prsimexpr *.prsimexprdiff
	-$(RM) *.prsimexpr-dot *.prsimexpr-ps

