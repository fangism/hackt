/**
	\file "fast-forward-test.v"
	Test case with set commands from a prsimrc source.
 */

`timescale 1ps/1ps 

module timeunit;
	initial $timeformat(-9,1," ns",9);
endmodule
module TOP;

	reg in;
	reg back;

	// prsim stuff
	initial 
	begin
	// @haco@ fast-forward-test.haco-c
		$prsim("fast-forward-test.haco-c");
		`include "addpath_srcdir.v"
		$prsim_cmd("echo $start of simulation");
		$prsim_cmd("watchall");

		$to_prsim("TOP.in",   "foo");
		$from_prsim("bar",   "TOP.back");
	#0	in <= 0;
	#5	$prsim_cmd("source fast-forward-test.prsimrc");
		$prsim_cmd("queue");
	#15	$finish;
	end

	always @(in) 
	begin
		$display("at time %7.3f, observed in %b", $realtime,in);
	end	

	always @(back) 
	begin
		$display("at time %7.3f, observed back %b", $realtime,back);
	end	

endmodule
