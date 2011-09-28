/**
	\file "channel-ledr-expect-fail.v"
	Chain a source and sink in prsim to verilog.
	$Id: $
 */

`timescale 1ns/1ps 

`include "standard.v"

module timeunit;
	initial $timeformat(-9,1," ns",9);
endmodule
module TOP;

	reg	l0, l1;
	wire	r0, r1;

	DELAY #(0.01) d0(l0, r0);
	DELAY #(0.01) d1(l1, r1);

	// prsim stuff
	initial 
	begin
	// @haco@ channel-ledr-source-sink.haco-c
		$prsim("channel-ledr-source-sink.haco-c");
		$prsim_cmd("echo $start of simulation");
		$prsim_cmd("echo-commands on");
		$prsim_cmd("mode fatal");

		$from_prsim("L.d","TOP.l0");
		$from_prsim("L.r","TOP.l1");
		$to_prsim("TOP.r0", "R.d");
		$to_prsim("TOP.r1", "R.r");
		$prsim_cmd("breaks");

		$prsim_cmd("watchall");
		$prsim_cmd("confirm");
		// $prsim_cmd("watchall-queue");
		$prsim_cmd("channel-ledr L : :0 d:0 r:0");
		$prsim_cmd("channel-ledr R : :0 d:0 r:0");
		$prsim_cmd("channel-timing L after 100");
		$prsim_cmd("channel-source-args-loop L 0 1 1 0 0 1");
//		$prsim_cmd("channel-sink R");
		$prsim_cmd("channel-expect-args-loop R 0 1 1 0 0 0");
//		$prsim_cmd("channel-expect-args R 0 1 1 0 0 0");
//		$prsim_cmd("channel-log R R.dat");
	
		$prsim_cmd("channel-watchall");
		$prsim_cmd("channel-ignore-all");
		$prsim_cmd("channel-reset-all");
//		$prsim_cmd("channel-show-all");
	#2
		$prsim_cmd("channel-release-all");
		$prsim_cmd("channel-heed-all");
//		$prsim_cmd("channel-show-all");
	#1
		$prsim_cmd("channel-show-all");
	#2
		$finish;
	end

endmodule
