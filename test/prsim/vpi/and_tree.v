// "and_tree.v"
// @vcs-flags@ -P pli.tab

`timescale 1ps / 1ps

// `include "standard.v"
`include "standard.v-wrap"

//-----------------------------------------------------------------------------
module timeunit;
	initial $timeformat(-9,1," ns",9);
endmodule

module TOP;
	reg a, b, c, d;
	wire z;
	initial
	begin
	// @haco@ and_tree.haco-c
		$prsim("and_tree.haco-c");
		$prsim_cmd("echo $start of simulation");
		$prsim_cmd("watchall");
		$to_prsim("TOP.a", "a");
		$to_prsim("TOP.b", "b");
		$to_prsim("TOP.c", "c");
		$to_prsim("TOP.d", "d");
		$from_prsim("z", "TOP.z");
	end

	// these could be automatically generated
	// by finding all globally unique instances of processes
	// along with their hierarchical names
	// e.g. from hacobjdupm of .haco-c file
	HAC_AND2 and_0();
	defparam and_0.prsim_name="mytree.and_0";
	HAC_AND2 and_1();
	defparam and_1.prsim_name="mytree.and_1";
	HAC_AND2 and_2();
	defparam and_2.prsim_name="mytree.and_2";


	initial
	begin
	#10	a <= 1'b0;
		b <= 1'b0;
		c <= 1'b0;
		d <= 1'b0;
	#100	a <= 1'b1;
		b <= 1'b1;
		c <= 1'b1;
		d <= 1'b1;
	#100	a <= 1'b0;
	#100	d <= 1'b0;
	#100	a <= 1'b1;
	#100	d <= 1'b1;
	#50	$finish;
	end
endmodule
