// "and_template_tree_reverse.v"
// @vcs-flags@ -P pli.tab

`timescale 1ps / 1ps

`include "standard-reverse.v-wrap"

//-----------------------------------------------------------------------------
module timeunit;
	initial $timeformat(-9,1," ns",9);
endmodule

module TOP;
	reg a, b, c, d;
	wire j, k, z;
	initial
	begin
	// @haco@ and_template_tree_reverse.haco-a
		$prsim("and_template_tree_reverse.haco-a");
		$prsim_cmd("echo $start of simulation");
		$prsim_cmd("watchall");
	end

	// these should be automatically generated
	// by finding all globally unique instances of processes
	// in the hierarchical RTL
	HAC_AND_N and_0({a, b}, j);
	defparam and_0.input_size=2;
	defparam and_0.prsim_name="__and_0";
	HAC_AND_N and_1({c, d}, k);
	defparam and_1.input_size=2;
	defparam and_1.prsim_name="__and_1";
	HAC_AND_N and_2({j, k}, z);
	defparam and_2.input_size=2;
	defparam and_2.prsim_name="__and_2";


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
