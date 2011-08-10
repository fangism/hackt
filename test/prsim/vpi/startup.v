/**
	\file "startup.v"
	Chain a bunch of inverters between VPI/VCS and prsim, shoelacing.
	$Id: inverters.v,v 1.3 2010/04/06 00:08:35 fang Exp $
	Thanks to Ilya Ganusov for contributing this test.
 */

`timescale 1ps/1ps 

`include "clkgen.v"

module timeunit;
	initial $timeformat(-9,1," ns",9);
endmodule
module TOP;

	reg in, in2;
	reg in3 = 1'b1;
	reg out, out2, out3;

	// prsim stuff
	initial 
	begin
	// @haco@ startup.haco-c
		$prsim("startup.haco-c");
		$prsim_cmd("echo $start of simulation");
		$prsim_cmd("getall .");
		$prsim_cmd("watchall");
		$prsim_cmd("watchall-queue");
		$prsim_cmd("set out0 1");
		in2 <= 1'b1;
	#5
		$prsim_cmd("echo -------------- connecting ---------------");
		$prsim_cmd("time");
		$to_prsim("TOP.in", "in0");
		$to_prsim("TOP.in2", "in1");
		$to_prsim("TOP.in3", "in2");
		$from_prsim("out0","TOP.out");
		$from_prsim("out1","TOP.out2");
		$from_prsim("!Vdd","TOP.out3");	// aliased to Vdd
		$prsim_cmd("time");
		$prsim_cmd("getall .");
	#5
		$prsim_cmd("echo -----------------------------");
		$prsim_cmd("time");
		in <= 1'b1;
		$prsim_cmd("set out1 1");
	#10
		$prsim_cmd("echo -----------------------------");
		$prsim_cmd("time");
		$prsim_cmd("getall .");
		$finish;
	end

	always @(in) 
	begin
		$display("at time %7.3f, observed in %b", $realtime,in);
	end	

	always @(in2) 
	begin
		$display("at time %7.3f, observed in2 %b", $realtime,in2);
	end	

	always @(in3) 
	begin
		$display("at time %7.3f, observed in3 %b", $realtime,in3);
	end	

	always @(out) 
	begin
		$display("at time %7.3f, observed out = %b", $realtime,out);
	end	

	always @(out2) 
	begin
		$display("at time %7.3f, observed out2 = %b", $realtime,out2);
	end	

	always @(out3) 
	begin
		$display("at time %7.3f, observed out3 = %b", $realtime,out3);
	end	

endmodule
