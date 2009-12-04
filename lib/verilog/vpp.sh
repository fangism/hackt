#!/bin/sh
#	$Id: vpp.sh,v 1.1.12.1 2009/12/04 19:27:52 fang Exp $
# crude verilog preprocessor, by Fang
# purpose is to crank a verilog file through a C-preprocessor
# by mapping verilog directives to C preprocessor directives.
# The primary application for this script is automatic wrapper generation
# through wrap_verilog_to_hacprsim.awk.

# usage: This script should be invoked as a pipe on both ends:
# vpp.sh < input.v > output.v

# flow:
# filter out some directives to ignore
# protect # characters
# cpp-ize #define, #if, #endif, etc...
# protect `True, `False verilog preprocessor constants
# protect naked single-quotes
# C-preprocess
# restore protected substitutions

# debugging tip:
# interrupt the pipe chain with a 'tee <file>' command to 
# examine the stream at any stage in the pipe sequence.

# protect single quote (doesn't work inside "string")
# sed -e "s/[']/\"\\\\'\"/g"

grep -v \
	-e '`timescale' \
	-e '`resetall' \
	-e '`celldefine' \
	-e '`endcell' \
	$1 | \
sed -e "s/[']/QUOTE/g" \
	-e 's/#/VPPPound/g' | \
sed -e 's/`define/#define/' \
	-e 's/`if/#if/' \
	-e 's/`endif/#endif/' \
	-e 's/`True/VPPTRUE/g' \
	-e 's/`False/VPPFalse/g' | \
sed 's/`\([a-zA-Z_][a-zA-Z_0-9]*\)/\1/g' | \
cpp -P | \
sed -e "s/QUOTE/'/g" \
	-e 's/VPPTrue/`True/g' \
	-e 's/VPPFalse/`False/g' \
	-e 's/VPPPound/#/g'

# restore single quote (doesn't work inside "string")
# sed -e "s/\\\"\\\\'\\\"/'/g"

