// "standard.v"
//	$Id: standard.v,v 1.4 2009/07/13 23:28:23 fang Exp $
// original verilog definitions (e.g. from vendor's standard cell library)

// two-input and-gate
`celldefine
module AND2 (A1, A2, Z);
    input A1, A2;
    output Z;
    and	(Z, A1, A2);		// plain AND gate
    specify
       (A1 => Z)=(5, 3);	// rise and fall times
       (A2 => Z)=(5, 3);
    endspecify
endmodule
`endcelldefine

// N-input and-gate
`celldefine
module AND_N (A, Z);
    parameter input_size = 2;
    input [input_size-1:0] A;
    output Z;
    and	(Z, A);			// plain AND gate
    specify
	(A => Z)=(4+input_size, 2+input_size);
    endspecify
endmodule
`endcelldefine

// two-input or gate
`celldefine
module OR2 (A1, A2, Z);
    input A1, A2;
    output Z;
    or	(Z, A1, A2);		// plain OR gate
    specify
       (A1 => Z)=(6, 4);	// rise and fall times
       (A2 => Z)=(6, 4);
    endspecify
endmodule
`endcelldefine

// an array/bus test
`celldefine
module bus_array_test(CLK, A, B, C, D, E, F, G, H);
	input		CLK;
	input 		A;
	input	[3:0] 	B;
	input 	[7:0]	C;
	input	[31:0]	D;
	output	reg		E;
	output	reg	[3:0] 	F;
	output 	reg	[7:0]	G;
	output	reg	[31:0]	H;
always @(posedge CLK)
begin
	E <= A;
	F <= B;
	G <= C;
	H <= D;
end
endmodule
`endcelldefine

