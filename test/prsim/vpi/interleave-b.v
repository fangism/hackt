/**
	\file "interleave-b.v"
	Chain a bunch of inverters between VPI/VCS and prsim, shoelacing.
	Added inverters in prsim to check for proper event interleaving.  
	$Id: interleave-b.v,v 1.2 2008/03/17 23:10:53 fang Exp $
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
	// @haco@ interleave-b.haco-a
		$prsim("interleave-b.haco-a");
		$prsim_cmd("echo $start of simulation");
		$prsim_cmd("watchall");
		$prsim_cmd("timing after");

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


	initial #6 $finish;

	initial
	$monitor("@%6.3f: out=%d,%d,%d,%d,%d", $realtime,
		out0, out1, out2, out3, out);

endmodule
