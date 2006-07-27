# "hackt.mk"
#	$Id: hackt.mk,v 1.3 2006/07/27 05:55:32 fang Exp $
# TODO: distinguish hackt-build.mk from hackt-inst.mk
# point these to hackt
top_srcdir = ../..
top_builddir = ../..
srcdir = .

# suffixes used by all test directories' Makefiles
# automake will convert these into .SUFFIXES:
.SUFFIXES: .hac .depend .hacktcmpltest .hacktobjtest .hacktunrolltest \
	.hacktcreatetest .hacktalloctest .hacktcflattest \
	.haco .haco-u .haco-c .haco-a .prs .sprs .prsimexpr .prsimexpr-dot \
	.prsimexpr-ps

# note: this points to the built hackt binary, but may be overriden
# to point to a different binary, or one in path
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

# this may be defined by the user
# HACO_FLAGS =

.hac.depend:
	$(HACKT_COMPILE_EXE) $(HACO_FLAGS) -M $@ $< 

# dependency tracking enabled by default
.hac.haco:
	depbase=`echo $@ | $(SED) 's/\.haco$$//g'` ; \
	if $(HACKT_COMPILE_EXE) $(HACO_FLAGS) -M "$$depbase.tmpd" $< $@ ; \
	then $(MV) "$$depbase.tmpd" "$$depbase.depend" ; \
	else $(RM) "$$depbase.tmpd" ; exit 1 ; \
	fi
#	$(HACKT_COMPILE_EXE) -I$(srcdir) $(HACO_FLAGS) $< $@

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

.haco-a.prsimexpr-dot:
	$(HACKT_PRSIM_EXE) -fno-run -fdump-dot-struct $< > $@

# assumes dot found in path
.prsimexpr-dot.prsimexpr-ps:
	dot -Tps $< -o $@

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

