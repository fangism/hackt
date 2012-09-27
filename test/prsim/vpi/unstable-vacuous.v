/**
	\file "unstable-vacuous.v"
	Debugging schedule-event-in-past internal error, 
	due to vacuous events being left in queue.
	$Id: $
 */

`timescale 1ps/1ps 
module timeunit;
	initial $timeformat(-9,1," ps",9);
endmodule

`timescale 1ps/1ps 
module TOP;

	reg	in, in2;
	wire	out, out2;
	reg	local;
/*
	reg	in3;
	wire	out3;
*/

	// prsim stuff
	initial 
	begin
	// @haco@ unstable-vacuous.haco-c
		$prsim("unstable-vacuous.haco-c");
		$prsim_cmd("echo $start of simulation");
		$prsim_cmd("timing after");
		$prsim_cmd("watchall");
		$prsim_cmd("unstable warn");
		$prsim_cmd("unstable-dequeue");

		$to_prsim("TOP.in",   "x");
		$to_prsim("TOP.in2",   "x2");
//		$to_prsim("TOP.in3",   "x3");
		$from_prsim("y","TOP.out");
		$from_prsim("y2","TOP.out2");
//		$from_prsim("y3","TOP.out3");
		$prsim_verbose_transport(1);
//		$vpi_dump_queue;	// this never shows anything!
	#100
		in <= 1'b0;		// enqueues y:1 @ 180
		in2 <= 1'b0;		// enqueues y2:1 @ 120
	#50
		local <= 1'b1;
	#150
		// expect vcs to report next next event time @ 300 (cb @ 120)
		// expect hit breakpt @ 120, return to vcs (cb @ 180)
		// b/c possible for event @ 180 to spawn new event before 300
		in2 <= 1'b1;		// enqueues y2:0 @ 320
		in <= 1'b1;		// enqueues y:0 @ 380
//		in3 <= 1'b1;
	#200
		$prsim_cmd("queue");
		// try glitching
	#10	in <= 1'b0;		// enqueues y:1 @ 590
		in2 <= 1'b0;		// enqueues y2:1 @ 530
		$prsim_cmd("queue");
	#10	in <= 1'b1;		// dequeues y (vacuous)
		$prsim_cmd("queue");
		// last event happens y2:1 @ 530
	#180
		$finish;
	end


	always @(in) 
	begin
		$display("at time %7.3f, observed in = %b", $realtime,in);
	end	
	always @(in2) 
	begin
		$display("at time %7.3f, observed in = %b", $realtime,in2);
	end	

	always @(out) 
	begin
		$display("at time %7.3f, observed out = %b", $realtime,out);
	end	
	always @(out2) 
	begin
		$display("at time %7.3f, observed out = %b", $realtime,out2);
	end	

	always @(local) 
	begin
		$display("at time %7.3f, observed local = %b", $realtime,local);
	end	


endmodule
