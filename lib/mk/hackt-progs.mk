# "mk/hackt-progs.mk"
#	vi: ft=make
#	$Id: hackt-progs.mk,v 1.4 2007/08/17 00:36:17 fang Exp $
# program variables

# NOTE: program names have not been transformed (TODO?)
HACKT_EXE = hackt
PARSE_TEST_EXE = $(HACKT_EXE) parse_test
# HACKT_COMPILE_EXE = $(HACKT_EXE) compile
HACKT_COMPILE_EXE = haco
# HACKT_OBJDUMP_EXE = $(HACKT_EXE) objdump
HACKT_OBJDUMP_EXE = hacobjdump
# HACKT_CREATE_EXE = $(HACKT_EXE) create
HACKT_CREATE_EXE = haccreate
# HACKT_ALLOC_EXE = $(HACKT_EXE) alloc
HACKT_ALLOC_EXE = hacalloc
# HACKT_CFLAT_EXE = $(HACKT_EXE) cflat
HACKT_CFLAT_EXE = hflat
HACKT_CFLAT_PRSIM_EXE = $(HACKT_CFLAT_EXE) prsim
HACKT_CFLAT_LVS_EXE = $(HACKT_CFLAT_EXE) lvs
# HACKT_PRSIM_EXE = $(HACKT_EXE) prsim
HACKT_PRSIM_EXE = hacprsim
# HACKT_CHPSIM_EXE = $(HACKT_EXE) chpsim
HACKT_CHPSIM_EXE = hacchpsim
HACKT_GUILE_EXE = hacguile
HACKT_CHPSIM_GUILE_EXE = hacchpsimguile

# for compile/link flags
HACKT_CONFIG = hackt-config
# pre-configure, installed libtool script
HACKT_LIBTOOL = hackt-libtool

CHPSIM_GRAPH_DOT = $(HACKT_CHPSIM_EXE) -fno-run -fdump-dot-struct
# recommended:
# CHPSIM_GRAPH_DOT_FLAGS = -fcluster-processes -fshow-channels
CHPSIM_GRAPH_DOT_COMMAND = $(CHPSIM_GRAPH_DOT) $(CHPSIM_GRAPH_DOT_FLAGS)

# from graphviz
DOT = dot
NEATO = neato
CIRCO = circo
TWOPI = twopi
FDP = fdp

# from transfig [graphicx]
FIG2DEV = fig2dev
