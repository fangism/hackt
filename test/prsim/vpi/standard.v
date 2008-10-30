// "standard.v"
// original verilog definitions (e.g. from vendor's standard cell library)

// two-input and-gate
module AND2 (A1, A2, Z);
    input A1, A2;
    output Z;
    and	(Z, A1, A2);		// plain AND gate
    specify
       (A1 => Z)=(5, 3);	// rise and fall times
       (A2 => Z)=(5, 3);
    endspecify
endmodule

// two-input or gate
module OR2 (A1, A2, Z);
    input A1, A2;
    output Z;
    or	(Z, A1, A2);		// plain OR gate
    specify
       (A1 => Z)=(6, 4);	// rise and fall times
       (A2 => Z)=(6, 4);
    endspecify
endmodule

