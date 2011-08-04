/**
	\file "channel-expect-fail.v"
	Chain a source and sink in prsim to verilog.
	$Id: $
 */

`timescale 1ns/1ps 

`include "standard.v"

module timeunit;
	initial $timeformat(-9,1," ns",9);
endmodule
module TOP;

	reg	l0, l1, re;
	wire	le, r0, r1;

	DELAY #(0.01) d0(l0, r0);
	DELAY #(0.01) d1(l1, r1);
	DELAY #(0.01) de(re, le);

	// prsim stuff
	initial 
	begin
	// @haco@ channel-source-sink.haco-c
		$prsim("channel-source-sink.haco-c");
		$prsim_cmd("echo $start of simulation");
		$prsim_cmd("echo-commands on");
		$prsim_cmd("mode fatal");

		$from_prsim("L.d[0]","TOP.l0");
		$from_prsim("L.d[1]","TOP.l1");
		$to_prsim("TOP.le", "L.e");
		$to_prsim("TOP.r0", "R.d[0]");
		$to_prsim("TOP.r1", "R.d[1]");
		$from_prsim("R.e", "TOP.re");
		$prsim_cmd("breaks");

		$prsim_cmd("watchall");
		// $prsim_cmd("watchall-queue");
		$prsim_cmd("channel L e:1 :0 d:2");
		$prsim_cmd("channel R e:1 :0 d:2");
		$prsim_cmd("channel-source-args-loop L 0 1 1 0 0 1");
		$prsim_cmd("channel-sink R");
		$prsim_cmd("channel-expect-args-loop R 0 1 1 0 0 0");
		$prsim_cmd("channel-watchall");
		$prsim_cmd("channel-reset-all");
//		$prsim_cmd("channel-show-all");
	#1
		$prsim_cmd("channel-release-all");
//		$prsim_cmd("channel-show-all");
	#5
		$prsim_cmd("channel-show-all");
		$finish;
	end

endmodule
