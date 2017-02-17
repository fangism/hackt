// a multidimensional port test
// FIXME: cosim doesn't connect to input A (but B works)
`celldefine
module three_dee(CLK, A, B, C, D, E, F, G, H);
	input		CLK;

	input 	[7:0]	A [0:2][0:3];
	output	reg	[4:0][5:0] 	B[0:1];

	input   [2:0]	C;
	output  [2:0]	D;

	input   	E [3:0];
	output  	F [3:0];

	input [3:0][2:0][1:0] G;
	output [3:0][2:0][1:0] H;
endmodule
`endcelldefine

`celldefine
module three_dee_alt(
	input		CLK,

	input 	[7:0]	A [0:2][0:3],
	output	reg	[4:0][5:0] 	B[0:1],

	input   [2:0]	C,
	output  [2:0]	D,

	input   	E [3:0],
	output  	F [3:0],

	input [3:0][2:0][1:0] G,
	output [3:0][2:0][1:0] H
);
endmodule
`endcelldefine
