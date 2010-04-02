// "param.v"
//	$Id: param.v,v 1.2 2010/04/02 22:30:05 fang Exp $
// original verilog definitions (e.g. from vendor's standard cell library)

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

