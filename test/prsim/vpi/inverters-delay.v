/**
	\file "inverters-delay.v"
	Modifying default delay value.  
	$Id: inverters-delay.v,v 1.2 2010/04/06 00:08:33 fang Exp $
 */

`timescale 1ns/1ps 

`include "clkgen.v"

module timeunit;
	initial $timeformat(-9,1," ns",9);
endmodule
module TOP;

	wire in;
	reg  out0, out1, out2, out3, out;
	clk_gen #(.HALF_PERIOD(1)) clk(in);	

	// prsim stuff
	initial 
	begin
	// @haco@ inverters.haco-c
		$prsim_default_after(25);	// in prsim time units
		$prsim("inverters.haco-c");
		$prsim_cmd("echo $start of simulation");
		$prsim_cmd("timing after");
		// $prsim_cmd("watchall");
		// $prsim_cmd("fanin out1");
		$to_prsim("TOP.in",   "in0");
		$to_prsim("TOP.out0", "in1");
		$to_prsim("TOP.out1", "in2");
		$to_prsim("TOP.out2", "in3");
		$to_prsim("TOP.out3", "in4");
		$from_prsim("out0","TOP.out0");
		$from_prsim("out1","TOP.out1");
		$from_prsim("out2","TOP.out2");
		$from_prsim("out3","TOP.out3");
		$from_prsim("out4","TOP.out");
	end

	initial #30 $finish;

	always @(in) 
	begin
		$display("at time %7.3f, observed in %b", $realtime,in);
	end	

	always @(out) 
	begin
		$display("at time %7.3f, observed out = %b", $realtime,out);
	end	

endmodule
