/**
	\file "shoelace.v"
	Chain a bunch of inverters between VPI/VCS and prsim, shoelacing.
	$Id: shoelace.v,v 1.3 2010/04/06 00:08:37 fang Exp $
	Thanks to Ilya Ganusov for contributing this test.
 */

`timescale 1ns/1ps 
`define inv_delay	0.010

`include "clkgen.v"

/* the humble inverter */
module inverter (in, out);
	parameter DELAY=`inv_delay;
	input in;
	output out;
	reg __o;
	wire out = __o;

	always @(in)
	begin
		#DELAY __o <= ~in;
	end
endmodule

module timeunit;
	initial $timeformat(-9,1," ns",9);
endmodule

/* our top-level */
module TOP;

	wire in, in1, in2, in3, in4;
	reg  out0, out1, out2, out3, out;
	clk_gen #(.HALF_PERIOD(1)) clk(in);	
/**
	assign in1 = ~out0;
	assign in2 = ~out1;
	assign in3 = ~out2;
	assign in4 = ~out3;
**/
	inverter q0(out0, in1);
	inverter q1(out1, in2);
	inverter q2(out2, in3);
	inverter q3(out3, in4);

	// prsim stuff
	initial 
	begin
	// @haco@ inverters.haco-c
		$prsim("inverters.haco-c");
		$prsim_cmd("echo $start of simulation");
		$prsim_confirm_connections();
		$prsim_verbose_transport(1);

		$to_prsim("TOP.in", "in0");
		$to_prsim("TOP.in1", "in1");
		$to_prsim("TOP.in2", "in2");
		$to_prsim("TOP.in3", "in3");
		$to_prsim("TOP.in4", "in4");
		$from_prsim("out0","TOP.out0");
		$from_prsim("out1","TOP.out1");
		$from_prsim("out2","TOP.out2");
		$from_prsim("out3","TOP.out3");
		$from_prsim("out4","TOP.out");
	end


	initial #5 $finish;

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
	end	

endmodule
