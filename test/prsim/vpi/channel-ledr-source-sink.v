/**
	\file "channel-ledr-source-sink.v"
	Chain a source and sink in prsim to verilog.
	$Id: channel-source-sink.v,v 1.4 2010/04/06 00:08:26 fang Exp $
 */

`timescale 1ns/1ps 

`include "standard.v"

`define inv_delay	0.020

module timeunit;
	initial $timeformat(-9,1," ns",9);
endmodule
module TOP;

	reg	l0, l1;
	wire	r0, r1;

	DELAY #(.delay(`inv_delay)) d0(l0, r0);
	DELAY #(.delay(`inv_delay)) d1(l1, r1);

	// prsim stuff
	initial 
	begin
	// @haco@ channel-ledr-source-sink.haco-c
		$prsim("channel-ledr-source-sink.haco-c");
		$prsim_cmd("echo $start of simulation");
		$prsim_cmd("echo-commands on");

		$from_prsim("L.d","TOP.l0");
		$from_prsim("L.r","TOP.l1");
		$to_prsim("TOP.r0", "R.d");
		$to_prsim("TOP.r1", "R.r");
		$prsim_cmd("breaks");

		$prsim_cmd("watchall");
		// $prsim_cmd("watchall-queue");
		$prsim_cmd("channel-ledr L : :0 d:0 r:0");
		$prsim_cmd("channel-ledr R : :0 d:0 r:0");
		$prsim_cmd("channel-source-args-loop L 0 1 1 0");
		$prsim_cmd("channel-expect-args-loop R 0 1 1 0");
		$prsim_cmd("channel-watchall");
		$prsim_cmd("channel-ignore-all");
		$prsim_cmd("channel-reset-all");
		$prsim_cmd("channel-show-all");
//		$prsim_cmd("breaks");

	#2	$prsim_cmd("channel-release-all");
		$prsim_cmd("channel-heed-all");
		$prsim_cmd("channel-show-all");
	#1	$prsim_cmd("nowatchall");
	#2	$finish;
	end

endmodule
