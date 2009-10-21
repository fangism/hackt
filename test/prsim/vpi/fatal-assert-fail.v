/**
	\file "fatal-assert-fail.v"
	Copy-modiifed from "inverters.v".
	$Id: fatal-assert-fail.v,v 1.1 2009/10/21 00:04:41 fang Exp $
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
	// @haco@ inverters.haco-a
		$prsim("inverters.haco-a");
		$prsim_cmd("echo $start of simulation");
		$prsim_cmd("watchall");

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


	initial #45 $finish;

/**
	// optional: produce vector file for dump
	initial begin
		$dumpfile ("test.dump"); 
		$dumpvars(0,TOP);
	end
**/

	always @(in) 
	begin
		$display("at time %7.3f, observed in %b", $realtime,in);
	end	

	always @(out) 
	begin
		$display("at time %7.3f, observed out = %b", $realtime,out);
		$prsim_cmd("assert out4 X");	// fail
	end	

endmodule
