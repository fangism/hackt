/**
	\file "initial-value.v"
	Testing that initial values are propagated to prsim
	upon connection with to_prsim.
	$Id: initial-value.v,v 1.2 2010/04/06 00:08:30 fang Exp $
 */

`timescale 1ps/1ps 

module timeunit;
	initial $timeformat(-9,1," ns",9);
endmodule

module TOP;

	wire in = 1'b1;
	reg  out0, out1, out2, out3, out;

	// prsim stuff
	initial 
	begin
	// @haco@ inverters.haco-c
		$prsim("inverters.haco-c");
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
	#100
		$prsim_cmd("getall .");
		$prsim_cmd("queue");
		$finish;
	end

endmodule
