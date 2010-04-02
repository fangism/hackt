// "parse-test.v"
// random verilog to make sure wrapper-generator script is robust

// definitions here are obfuscated garbage
module T_001 (a, ck, en, z_d0, z_d1, z_ack);
parameter mode = "locked";
parameter i_k_polarity = "rise";
parameter i_k_dl = 0;
parameter dmn = "";
parameter data_is_reset = 0;
parameter e_dls = 1;
parameter ig_z_ack = 0;
input     a;
input     ck;
input     en;
input     z_ack;
output    z_d0, z_d1;
reg       e_dls_reg = `FOOBAR;
reg       z_d0_reg      = 1'b0;
reg       ck_dl, ck_int_dl;
reg       late             = 1'b0;
wire      ck_int;
tri1      en_int;
wire a_b;
wire z_ack_b;
wire en_b;
buf(a_b, din);
generate
   if ( ig_z_ack )
     begin
        always @(*)
          begin
             $display("ERROR: ig_z_ack not supported on SACs!");
             $finish;
          end
     assign z_ack_b = 1'bz;
   end else
     buf(z_ack_b, z_ack);
endgenerate
endmodule

