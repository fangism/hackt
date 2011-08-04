// "standard.v"
//	$Id: standard.v,v 1.5 2009/10/22 22:30:36 fang Exp $
// original verilog definitions (e.g. from vendor's standard cell library)

// inverter
`celldefine
module INV (A, Z);
  input A;
  output Z;
  not (Z, A);
  specify
    (A => Z)=(3, 3);	// rise and fall times
  endspecify
endmodule
`endcelldefine

// delay element
`celldefine
module DELAY (A, Z);
  parameter delay = 10;
  input A;
  output Z;
  assign #delay Z = A;
endmodule
`endcelldefine

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

`celldefine
// testing ignore extra wire keyword
module wire_port_test(
input wire  in_a,
input wire  [3:0] in_b,
output wire  [3:0]  out_x);

endmodule
`endcelldefine

