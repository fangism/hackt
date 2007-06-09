# "mk/hackt-rules.mk"
#	vi: ft=automake
#	$Id: hackt-rules.mk,v 1.1 2007/06/09 01:56:30 fang Exp $
# The rules portion of the hackt automake template.
# The counterpart of this file is "mk/hackt-suffixes.am".
# Include this file after suffixes have been included.  

# NOTE: prerequisite variable definitions:
#	RM, MV, SED
# optional variables:
#	HACO_FLAGS
# VPATH-builds will need "-I$(srcdir)"

# with dependency tracking enabled by default
.hac.depend:
	$(HACKT_COMPILE_EXE) $(HACO_FLAGS) -M $@ $< 

.hac.haco:
	depbase=`echo $@ | $(SED) 's/\.haco$$//g'` ; \
	if $(HACKT_COMPILE_EXE) $(HACO_FLAGS) -M "$$depbase.tmpd" $< $@ ; \
	then $(MV) "$$depbase.tmpd" "$$depbase.depend" ; \
	else $(RM) "$$depbase.tmpd" ; exit 1 ; \
	fi
#	$(HACKT_COMPILE_EXE) -I$(srcdir) $(HACO_FLAGS) $< $@

# .haco.haco-u:
#	$(HACKT_UNROLL_EXE) $< $@

# .haco-u.haco-c:
#	$(HACKT_CREATE_EXE) $< $@

.haco.haco-c:
	$(HACKT_CREATE_EXE) $< $@

.haco-c.haco-a:
	$(HACKT_ALLOC_EXE) $< $@

.haco-a.prs:
	$(HACKT_CFLAT_PRSIM_EXE) $< > $@

.haco-a.lvsprs:
	$(HACKT_CFLAT_LVS_EXE) $< > $@

.haco-a.sprs:
	$(HACKT_CFLAT_PRSIM_EXE) -fsizes $< > $@

.haco-a.lvssprs:
	$(HACKT_CFLAT_LVS_EXE) -fsizes $< > $@

# note: does not use optimization
.haco-a.prsimexpr:
	$(HACKT_PRSIM_EXE) -fno-run -fdump-expr-alloc $< > $@

.haco-a.prsimexpr-O1:
	$(HACKT_PRSIM_EXE) -fno-run -fdump-expr-alloc -O1 $< > $@

# note: does not use optimization
.haco-a.prs-dot:
	$(HACKT_PRSIM_EXE) -fno-run -fdump-dot-struct $< > $@

.haco-a.prs-dot-O1:
	$(HACKT_PRSIM_EXE) -fno-run -fdump-dot-struct -O1 $< > $@

# assumes dot and friends are found in path
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

