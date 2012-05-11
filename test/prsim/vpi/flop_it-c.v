// "flop_it-c.v"
// @vcs-flags@ -P pli.tab

`timescale 1ps / 1ps

// `include "standard.v"
`include "standard.v-wrap"
`include "clkgen.v"

//-----------------------------------------------------------------------------
module timeunit;
	initial $timeformat(-9,1," ns",9);
endmodule

module TOP;
	reg a;
	wire clk;
	wire z;
	clk_gen #(.HALF_PERIOD(50)) cg(clk);	
	initial
	begin
	// @haco@ flop_it.haco-c
		$prsim("flop_it.haco-c");
		$prsim_cmd("echo $start of simulation");
		$prsim_cmd("watchall");
		$to_prsim("TOP.a", "a");
		$to_prsim("TOP.clk", "clk");
		$from_prsim("z", "TOP.z");
	end

	// these could be automatically generated
	// by finding all globally unique instances of processes
	// along with their hierarchical names
	// e.g. from hacobjdump of .haco-c file

	// test using escaped identifiers
	HAC_POS_FLOP #(.prsim_name("F.f[0]")) \F.f[0] ();
	HAC_POS_FLOP #(.prsim_name("F.f[1]")) \F.f[1] ();
	HAC_POS_FLOP #(.prsim_name("F.f[2]")) \F.f[2] ();
	HAC_POS_FLOP #(.prsim_name("F.f[3]")) \F.f[3] ();

	initial
	begin
	#20	a <= 1'b0;
	#400	a <= 1'b1;
	#400	a <= 1'b0;
	#100	a <= 1'b1;
	#100	a <= 1'b0;
	#100	a <= 1'b1;
	#300	a <= 1'b0;
	#50	$finish;
	end
endmodule
