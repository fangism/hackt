// "multidimensional-a.v"
// @vcs-flags@ -P pli.tab -sverilog
// -sverilog to support multidimensional arrays

`timescale 1ps / 1ps

`include "multidimensional-reverse.v-wrap"
	// wrapper needs repack_arrays=1 workaround

`include "clkgen.v"

//-----------------------------------------------------------------------------
module _timeunit;
	initial $timeformat(-9,1," ns",9);
endmodule

module TOP;
	reg a;
	wire clk;
	wire z1, z2;
	// clk_gen #(.HALF_PERIOD(80)) ag(a);	
	clk_gen #(.HALF_PERIOD(50)) cg(clk);	
	initial
	begin
		$prsim_confirm_connections();
		$prsim_verbose_transport(1);
		// @haco@ multidimensional-a.haco-c
		$prsim("multidimensional-a.haco-c");
		$display("$start of simulation");
		$prsim_cmd("watchall");
	end

	HAC_three_dee f0();
	defparam f0.prsim_name="foo_1";
	assign f0.CLK = clk;
	// assign f0.A[1][2] = {8{a}};	// only care about [1][2][3]
	// assign z = f0.B[1][2][3];
	assign f0.C[1] = a;		// bus works
	assign z1 = f0.D[1];
	// assign f0.E[2] = a;		// array fails, no input transported
	// assign z = f0.F[2];
	assign f0.G[1][2][0] = a;	// only care about [1][2][3]
	assign z2 = f0.H[1][2][0];

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

	always @(a) 
	begin
		$display("at time %7.3f, observed a = %b", $realtime, a);
	end	

	always @(z1) 
	begin
		$display("at time %7.3f, observed z1 = %b", $realtime, z1);
	end	

	always @(z2) 
	begin
		$display("at time %7.3f, observed z2 = %b", $realtime, z2);
	end	

endmodule
