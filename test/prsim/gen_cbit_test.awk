#!/usr/bin/awk -f
# "gen_cbit_test.awk"
# no input, generates prsim script

BEGIN {
	print "initialize";
	print "weak-rules on";
	print "set write 0";
	print "set _pchg 0";
	print "set din 0";
	print "set bl 1";
	print "set _bl 1";
	print "mode reset";
	print "unstable-dequeue";
	print "unstable warn";

	for (i=0; i<4; ++i) {
		printf("set wl[%d] 0\n", i);
	}
	print "cycle"

	for (i=0; i<4; ++i) {
		printf("set p[%d].t 0\n", i);
		printf("set p[%d].f 1\n", i);
	}
	print "cycle"

	j = 0;
	val = 0;
	for (i=0; i<100; ++i) {
		# or rand()
		addr = (j*val/3) % 4;
		val = ((1+j)*3/5) % 2;

		printf("echo Writing %d to addr %d\n", val, addr);
		print "set _pchg 1";
		print "cycle";
		printf("set din %d\n", val);
		print "cycle";
		print "set write 1";
		print "cycle";
		# Sanity-check bitline

		if (val >= 1) {
			print "assert bl 1";
			print "assert _bl 0";
		} else {
			print "assert bl 0";
			print "assert _bl 1";
		}

		printf("set wl[%d] 1\n", addr);
		print "cycle";
		printf("assert p[%d].t %d\n", addr, val);

		# Read back
		printf("set wl[%d] 0\n", addr);
		print "cycle";
		print "set write 0";
		print "cycle";
		printf("set wl[%d] 1\n", addr);
		print "cycle";

		if (val >= 1) {
			print "assert bl 1";
			print "assert _bl 0";
		} else {
			print "assert bl 0";
			print "assert _bl 1";
		}
		printf("set wl[%d] 0\n", addr);
		print "cycle";
		print "set _pchg 0";
		print "cycle";
		++j;
	}
}

