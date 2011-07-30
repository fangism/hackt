/**
	\file "celem-test.v"
	Drive a single C-element with phase-shifted clocks
	Expect to see missing keeper diagnostics.
	$Id: inverters.v,v 1.3 2010/04/06 00:08:35 fang Exp $
 */

`timescale 1ns/1ps 

`include "clkgen.v"

module timeunit;
	initial $timeformat(-9,1," ns",9);
endmodule
module TOP;

	wire in0, in1;
	reg  out;
	clk_gen #(.HALF_PERIOD(1)) clk0(in0);
	clk_gen #(.HALF_PERIOD(1), .PHASE_SHIFT(0.5)) clk1(in1);

	// prsim stuff
	initial 
	begin
	// @haco@ celem.haco-c
		$prsim_options("-f fast-weak-keepers");
		$prsim("celem.haco-c");
		$prsim_cmd("echo $start of simulation");
		$prsim_cmd("weak-rules on");
		$prsim_cmd("keeper-check-fail warn");

		$to_prsim("TOP.in0",   "in0");
		$to_prsim("TOP.in1",   "in1");
		$from_prsim("out","TOP.out");
	end


	initial #15 $finish;

	always @(in0) 
	begin
		$display("at time %7.3f, observed in0 %b", $realtime,in0);
	end	

	always @(in1) 
	begin
		$display("at time %7.3f, observed in1 %b", $realtime,in1);
	end	

	always @(out) 
	begin
		$display("at time %7.3f, observed out = %b", $realtime,out);
	end	

endmodule
