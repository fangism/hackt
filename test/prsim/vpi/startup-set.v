/**
	\file "startup-set.v"
	Test case with set commands from a prsimrc source.
 */

`timescale 1ps/1ps 

`include "clkgen.v"

module timeunit;
	initial $timeformat(-9,1," ns",9);
endmodule
module TOP;

	wire in;
	reg back, probe, g1, g2;
	clk_gen #(.HALF_PERIOD(10), .PHASE_SHIFT(2)) clk(in);	

	// prsim stuff
	initial 
	begin
	// @haco@ startup-set.haco-c
		$prsim("startup-set.haco-c");
		`include "addpath_srcdir.v"
		$prsim_cmd("echo $start of simulation");
		$prsim_cmd("watchall");

		$to_prsim("TOP.in",   "foo");
		$from_prsim("foo",   "TOP.back");
		$from_prsim("Vdd1",   "TOP.probe");
		$from_prsim("GND",   "TOP.g1");
		$from_prsim("GND",   "TOP.g2");
	#5 $prsim_cmd("source startup-set.prsimrc");
		$prsim_cmd("queue");
	#40 $finish;
	end

	always @(in) 
	begin
		$display("at time %7.3f, observed in %b", $realtime,in);
	end	

	always @(back) 
	begin
		$display("at time %7.3f, observed back %b", $realtime,back);
	end	

	always @(probe) 
	begin
		$display("at time %7.3f, observed probe %b", $realtime,probe);
	end	

	always @(g1) 
	begin
		$display("at time %7.3f, observed g1 %b", $realtime,g1);
	end	

	always @(g2) 
	begin
		$display("at time %7.3f, observed g2 %b", $realtime,g2);
	end	

endmodule
