# "mk/hackt-rules.mk"
#	vi: ft=automake
#	$Id: hackt-rules.mk,v 1.4 2007/08/17 00:36:18 fang Exp $
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
	$(DOT) -Tps $< -o $@

.prs-dot-O1.prs-dot-fig:
	$(DOT) -Tfig $< -o $@

.prs-dot-O1.prs-neato-ps:
	$(NEATO) -Tps $< -o $@

.prs-dot-O1.prs-neato-fig:
	$(NEATO) -Tfig $< -o $@

.prs-dot-O1.prs-circo-ps:
	$(CIRCO) -Tps $< -o $@

.prs-dot-O1.prs-circo-fig:
	$(CIRCO) -Tfig $< -o $@

.prs-dot-O1.prs-twopi-ps:
	$(TWOPI) -Tps $< -o $@

.prs-dot-O1.prs-twopi-fig:
	$(TWOPI) -Tfig $< -o $@

.prs-dot-O1.prs-fdp-ps:
	$(FDP) -Tps $< -o $@

.prs-dot-O1.prs-fdp-fig:
	$(FDP) -Tfig $< -o $@

.prs-dot-fig.prs-dot-pdf:
	$(FIG2DEV) -Lpdf $< $@

.prs-neato-fig.prs-neato-pdf:
	$(FIG2DEV) -Lpdf $< $@

.prs-circo-fig.prs-circo-pdf:
	$(FIG2DEV) -Lpdf $< $@

.prs-twopi-fig.prs-twopi-pdf:
	$(FIG2DEV) -Lpdf $< $@

.prs-fdp-fig.prs-fdp-pdf:
	$(FIG2DEV) -Lpdf $< $@

.haco-a.chpsim-event-dot:
	@echo "$(CHPSIM_GRAPH_DOT_COMMAND) $< > $@" ; \
	if $(CHPSIM_GRAPH_DOT_COMMAND) $< > $@.tmp ; then $(MV) $@.tmp $@ ; \
	else $(RM) $@.tmp ; exit 1 ; \
	fi

.chpsim-event-dot.chpsim-event-fig:
	$(DOT) -Tfig $< -o $@

.chpsim-event-fig.chpsim-event-pdf:
	$(FIG2DEV) -Lpdf $< $@

.chpsim-event-dot.chpsim-event-ps:
	$(DOT) -Tps $< -o $@



