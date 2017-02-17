// "unpacked_packed.v"
// testing element-by-element repacking of arrays
// repacking is a workaround to some bug that prevents bus-declared
// input ports (e.g. input X [1:0]) from propagating to prsim (cosim) properly.
// Workaround is implemented in wrap_verilog_modules_to_hacprsim.awk.
// See multidimensional.v.

// @vcs-flags@ -P pli.tab -sverilog
// -sverilog to support multidimensional arrays

`timescale 1ps / 1ps

`include "clkgen.v"

//-----------------------------------------------------------------------------
module _timeunit;
	initial $timeformat(-9,1," ns",9);
endmodule

module ar1(
	input wire [3:0] A[1:0][2:0],
	output wire [3:0] B[1:0][2:0]
);
	assign B = A;
endmodule

module ar2(
	input wire [2:0][3:0] A[1:0],
	output wire [2:0][3:0] B[1:0]
);
	// ar1 foo(A);	// works?  no
	ar1 foo();
	genvar i, j, k;
	for (i=0; i<2; ++i) begin : i_loop
	for (j=0; j<3; ++j) begin : j_loop
	for (k=0; k<4; ++k) begin : k_loop
		assign foo.A[i][j][k] = A[i][j][k];
		assign B[i][j][k] = foo.B[i][j][k];
	end
	end
	end
endmodule

module ar3(
	input wire [1:0][2:0][3:0] A,
	output wire [1:0][2:0][3:0] B
);
	// ar1 foo(A);	// works?  no, mismatched arrays
	ar2 foo();
	genvar i, j, k;
	for (i=0; i<2; ++i) begin : i_loop
	for (j=0; j<3; ++j) begin : j_loop
	for (k=0; k<4; ++k) begin : k_loop
		assign foo.A[i][j][k] = A[i][j][k];
		assign B[i][j][k] = foo.B[i][j][k];
	end
	end
	end
endmodule

module TOP;
	wire clk;
	wire z;
	clk_gen #(.HALF_PERIOD(50)) cg(clk);	

	ar3 d2();
	assign d2.A[1][2][3] = clk;	// works (swizzle)
	assign z = d2.B[1][2][3];

	initial
	begin
		$display("$start of simulation");
	#850	$finish;
	end

	always @(clk) 
	begin
		$display("at time %7.3f, observed clk = %b", $realtime, clk);
	end	

	always @(z) 
	begin
		$display("at time %7.3f, observed z = %b", $realtime, z);
	end	

endmodule
