/**
	\file "oscillator-fanout.v"
	A prsim-driven oscilator faning out to multiple verilog nodes.
	This is a test case for a bug as well.  
	$Id: oscillator-fanout.v,v 1.2 2010/04/06 00:08:36 fang Exp $
 */

`timescale 1ns/1ps 

module timeunit;
	initial $timeformat(-9,1," ns",9);
endmodule
module TOP;

	reg [3:0] out;
	// reg out0, out1, out2, out3;

	// prsim stuff
	initial 
	begin
	// @haco@ oscillator.haco-c
		$prsim("oscillator.haco-c");
		$from_prsim("R.x[7]","TOP.out[0]");
		$from_prsim("R.x[7]","TOP.out[1]");
		$from_prsim("R.x[7]","TOP.out[2]");
		$from_prsim("R.x[7]","TOP.out[3]");

		$prsim_cmd("echo $start of simulation");
		$prsim_cmd("timing after");
		$prsim_cmd("watchall");
		$prsim_cmd("set _Reset 0");
	#0.2	$prsim_cmd("set _Reset 1");
	#1	$finish;
	end

initial $monitor("at time %5.3f, out[0..3] = %b, %b, %b, %b",
		$time, out[0], out[1], out[2], out[3]);

endmodule
