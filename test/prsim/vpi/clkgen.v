/**
	\file "clkgen.v"
	$Id: clkgen.v,v 1.2 2008/03/17 23:10:50 fang Exp $
 */

/// a clock generator
module clk_gen(clk);
	parameter HALF_PERIOD = 2;
	parameter PHASE_SHIFT = 0.0;

	output reg clk;

	initial begin
		clk <= 1'b0;
		#PHASE_SHIFT 
		forever #(HALF_PERIOD) clk = ~clk;
	end
endmodule

