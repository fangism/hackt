/**
	\file "independent-01.v"
	Completely decoupled simulations.  
	$Id: independent-01.v,v 1.2 2008/03/17 23:10:51 fang Exp $
	Thanks to Ilya Ganusov for contributing this test.
 */

`timescale 1ns/1ps 

`include "clkgen.v"

module timeunit;
	initial $timeformat(-9,1," ns",9);
endmodule
module TOP;

	wire in;
	/* reg  out0, out1, out2, out3, out; */
	clk_gen #(.HALF_PERIOD(0.1)) clk(in);	

	// prsim stuff
	initial 
	begin
	// @haco@ oscillator.haco-a
		$prsim("oscillator.haco-a");
		$prsim_cmd("echo $start of simulation");
		$prsim_cmd("echo-commands on");
		$prsim_cmd("watchall");
	end

	initial #0.050	$prsim_sync();
	/** schedules to prsim's event queue **/
	initial #0.050	$prsim_cmd("set _Reset 0");
	/** but VCS doesn't know about it, so we have to synchronize **/
	initial #0.050	$prsim_sync();

	initial #0.200	$prsim_cmd("set _Reset 1");
	initial #0.200	$prsim_sync();

	initial #2 $finish;

/**
	// optional: produce vector file for dump
	initial begin
		$dumpfile ("test.dump"); 
		$dumpvars(0,TOP);
	end
**/

	always @(in) 
	begin
		$display("at time %6.3f, observed in %b", $realtime, in);
	end	

endmodule
