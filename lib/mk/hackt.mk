# "hackt.mk"
#	$Id: hackt.mk,v 1.5 2006/07/28 21:30:03 fang Exp $
# TODO: distinguish hackt-build.mk from hackt-inst.mk
# point these to hackt
top_srcdir = ../..
top_builddir = ../..
srcdir = .

# suffixes used by all test directories' Makefiles
# automake will convert these into .SUFFIXES:
.SUFFIXES: .hac .depend .hacktcmpltest .hacktobjtest .hacktunrolltest \
	.hacktcreatetest .hacktalloctest .hacktcflattest \
	.haco .haco-u .haco-c .haco-a .prs .sprs \
	.prsimexpr .prsimexpr-O1 .prs-dot .prs-dot-O1 \
	.prs-dot-ps .prs-neato-ps .prs-circo-ps .prs-twopi-ps .prs-fdp-ps \
	.prs-dot-fig .prs-neato-fig .prs-circo-fig .prs-twopi-fig .prs-fdp-fig \
	.prs-dot-pdf .prs-neato-pdf .prs-circo-pdf .prs-twopi-pdf .prs-fdp-pdf

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

.haco-a.prsimexpr-O1:
	$(HACKT_PRSIM_EXE) -fno-run -fdump-expr-alloc -O1 $< > $@

.haco-a.prs-dot:
	$(HACKT_PRSIM_EXE) -fno-run -fdump-dot-struct $< > $@

.haco-a.prs-dot-O1:
	$(HACKT_PRSIM_EXE) -fno-run -fdump-dot-struct -O1 $< > $@

# assumes dot and friends found in path
# uses optimized expression netlist by default
.prs-dot-O1.prs-dot-ps:
	dot -Tps $< -o $@

.prs-dot-O1.prs-dot-fig:
	dot -Tfig $< -o $@

.prs-dot-O1.prs-neato-ps:
	neato -Tps $< -o $@

.prs-dot-O1.prs-neato-fig:
	neato -Tfig $< -o $@

.prs-dot-O1.prs-circo-ps:
	circo -Tps $< -o $@

.prs-dot-O1.prs-circo-fig:
	circo -Tfig $< -o $@

.prs-dot-O1.prs-twopi-ps:
	twopi -Tps $< -o $@

.prs-dot-O1.prs-twopi-fig:
	twopi -Tfig $< -o $@

.prs-dot-O1.prs-fdp-ps:
	fdp -Tps $< -o $@

.prs-dot-O1.prs-fdp-fig:
	fdp -Tfig $< -o $@

.prs-dot-fig.prs-dot-pdf:
	fig2dev -Lpdf $< $@

.prs-neato-fig.prs-neato-pdf:
	fig2dev -Lpdf $< $@

.prs-circo-fig.prs-circo-pdf:
	fig2dev -Lpdf $< $@

.prs-twopi-fig.prs-twopi-pdf:
	fig2dev -Lpdf $< $@

.prs-fdp-fig.prs-fdp-pdf:
	fig2dev -Lpdf $< $@

all:

clean: clean-local

clean-local:
	-$(RM) *.haco
	-$(RM) *.haco-u *.unrolldump
	-$(RM) *.haco-c *.createdump
	-$(RM) *.haco-a *.allocdump
	-$(RM) *.prs
	-$(RM) *.prsimexpr*
	-$(RM) *.prs-dot* *.prs-*-ps
	-$(RM) *.prs-*-fig *.prs-*-pdf
	-$(RM) *.sprs

