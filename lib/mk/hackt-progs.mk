# "mk/hackt-progs.mk"
#	vi: ft=make
#	$Id: hackt-progs.mk,v 1.1 2007/06/09 01:56:30 fang Exp $
# program variables

# NOTE: program names have not been transformed (TODO?)
HACKT_EXE = hackt
PARSE_TEST_EXE = $(HACKT_EXE) parse_test
# HACKT_COMPILE_EXE = $(HACKT_EXE) compile
HACKT_COMPILE_EXE = haco
# HACKT_OBJDUMP_EXE = $(HACKT_EXE) objdump
HACKT_OBJDUMP_EXE = hacobjdump
# HACKT_UNROLL_EXE = $(HACKT_EXE) unroll
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

