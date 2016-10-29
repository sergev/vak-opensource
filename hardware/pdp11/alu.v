`timescale 1ns / 1ps
//
// ALU.
//
`include "opcode.v"

module alu (
    input wire [9:0] op,	// op code
    input wire [15:0] a,	// `source' register
    input wire [15:0] b,	// `destination' register
    input wire [7:0] ps_in,	// processor state register
    output reg [15:0] d,	// result
    output reg [7:0] ps_out	// processor state result
);

`define n_in	ps_in[3]	// negative
`define z_in	ps_in[2]	// zero
`define v_in	ps_in[1]	// overflow
`define c_in	ps_in[0]	// carry

`define n_out   ps_out[3]       // negative result
`define z_out   ps_out[2]       // zero result
`define v_out   ps_out[1]       // overflow result
`define c_out   ps_out[0]       // carry result

    // PC offset for branch instructions
    // PC offset for branch instructions
    wire [15:0] offset = { a[7], a[7], a[7], a[7], a[7], a[7], a[7], a[7:0], 1'b0 };

    always @(op or a or b or ps_in or d) begin
        ps_out = ps_in;
        casez (op)
        default: begin
            d = b;
        end

        //
        // Single-operand instructions.
        //
        `CLR: begin		// d = 0
            d = 0;
            `c_out = 0;
            `v_out = 0;
            `n_out = d[15];
            `z_out = (d == 0);
        end
        `CLRB: begin
            d = { b[15:8], 8'd0 };
            `c_out = 0;
            `v_out = 0;
            `n_out = d[7];
            `z_out = (d[7:0] == 0);
        end
        `COM: begin		// d = ~b
            d = ~b;
            `c_out = 1;
            `v_out = 0;
            `n_out = d[15];
            `z_out = (d == 0);
        end
        `COMB: begin
            d = { b[15:8], ~b[7:0] };
            `c_out = 1;
            `v_out = 0;
            `n_out = d[7];
            `z_out = (d[7:0] == 0);
        end
        `INC: begin		// d = b + 1
            d = b + 1;
            `v_out = (b == 16'h7fff);
            `n_out = d[15];
            `z_out = (d == 0);
        end
        `INCB: begin
            d = { b[15:8], (b[7:0] + 1'd1) };
            `v_out = (b[7:0] == 8'h7f);
            `n_out = d[7];
            `z_out = (d[7:0] == 0);
        end
        `INC2: begin		// d = b + 2 (nonstandard)
            d = b + 2;
        end
        `DEC: begin		// d = b - 1
            d = b - 1;
            `v_out = (b == 16'h8000);
            `n_out = d[15];
            `z_out = (d == 0);
        end
        `DECB: begin
            d = { b[15:8], (b[7:0] - 1'd1) };
            `v_out = (b[7:0] == 8'h80);
            `n_out = d[7];
            `z_out = (d[7:0] == 0);
        end
        `DEC2: begin		// d = b - 2 (nonstandard)
            d = b - 2;
        end
        `NEG: begin		// d = 0 - b
            d = - b;
            `c_out = (d != 0);
            `v_out = (d == 16'h8000);
            `n_out = d[15];
            `z_out = (d == 0);
        end
        `NEGB: begin
            d = { b[15:8], -b[7:0] };
            `c_out = (d != 0);
            `v_out = (d[7:0] == 8'h80);
            `n_out = d[7];
            `z_out = (d[7:0] == 0);
        end
        `TST: begin		// d = b
            d = b;
            `c_out = 0;
            `v_out = 0;
            `n_out = d[15];
            `z_out = (d == 0);
        end
        `TSTB: begin
            d = b;
            `c_out = 0;
            `v_out = 0;
            `n_out = d[7];
            `z_out = (d[7:0] == 0);
        end
        `ASR: begin		// d|c = b >> 1
            { d, `c_out } = { b[15], b };
            `v_out = d[15] ^ `c_out;
            `n_out = d[15];
            `z_out = (d == 0);
        end
        `ASRB: begin
            { d, `c_out } = { b[15:8], b[7], b[7:0] };
            `v_out = d[7] ^ `c_out;
            `n_out = d[7];
            `z_out = (d[7:0] == 0);
        end
        `ASL: begin		// c|d = b << 1
            { `c_out, d } = { b, 1'd0 };
            `v_out = d[15] ^ `c_out;
            `n_out = d[15];
            `z_out = (d == 0);
        end
        `ASLB: begin
            { d[15:8], `c_out, d[7:0] } = { b, 1'd0 };
            `v_out = d[7] ^ `c_out;
            `n_out = d[7];
            `z_out = (d[7:0] == 0);
        end
        `ROR: begin		// d|c = c|b
            { d, `c_out } = { `c_in, b };
            `v_out = d[15] ^ `c_out;
            `n_out = d[15];
            `z_out = (d == 0);
        end
        `RORB: begin
            { d, `c_out } = { b[15:8], `c_in, b[7:0] };
            `v_out = d[7] ^ `c_out;
            `n_out = d[7];
            `z_out = (d[7:0] == 0);
        end
        `ROL: begin		// c|d = b|c
            { `c_out, d } = { b, `c_in };
            `v_out = d[15] ^ `c_out;
            `n_out = d[15];
            `z_out = (d == 0);
        end
        `ROLB: begin
            { d[15:8], `c_out, d[7:0] } = { b, `c_in };
            `v_out = d[7] ^ `c_out;
            `n_out = d[7];
            `z_out = (d[7:0] == 0);
        end
        `SWAB: begin		// d = swab (b)
            d = { b[7:0], b[15:8] };
            `c_out = 0;
            `v_out = 0;
            `n_out = d[15];
            `z_out = (d == 0);
        end
        `ADC: begin		// d = b + c
            d = b + {15'h0, `c_in};
            `c_out = (b == 16'hffff && `c_in == 1);
            `v_out = (b == 16'h7fff && `c_in == 1);
            `n_out = d[15];
            `z_out = (d == 0);
        end
        `ADCB: begin
            d = { b[15:8], (b[7:0] + {7'h0, `c_in}) };
            `c_out = (b[7:0] == 8'hff && `c_in == 1);
            `v_out = (b[7:0] == 8'h7f && `c_in == 1);
            `n_out = d[7];
            `z_out = (d[7:0] == 0);
        end
        `SBC: begin		// d = b - c
            d = b - {15'h0, `c_in};
            `c_out = (b == 0) && (`c_in != 0);
            `v_out = (b == 16'h8000);
            `n_out = d[15];
            `z_out = (d == 0);
        end
        `SBCB: begin
            d = { b[15:8], (b[7:0] - {7'h0, `c_in}) };
            `c_out = (b[7:0] == 0) && (`c_in != 0);
            `v_out = (b[7:0] == 8'h80);
            `n_out = d[7];
            `z_out = (d[7:0] == 0);
        end
        `SXT: begin		// d = n ? -1 : 0
            d = `n_in ? 16'hffff : 0;
            `v_out = 0;
            `n_out = d[15];
            `z_out = (d == 0);
        end
        `MFPS: begin		// d = ps
            d = { ps_in[7], ps_in[7], ps_in[7], ps_in[7],
                    ps_in[7], ps_in[7], ps_in[7], ps_in[7], ps_in };
            `v_out = 0;
            `n_out = d[7];
            `z_out = (d[7:0] == 0);
        end
        `MTPS: begin		// ps = b
            d = b;
            ps_out = { b[7:5], ps_in[4], b[3:0] };
        end

        //
        // Double-operand instructions.
        //
        `MOV: begin		// d = a
            d = a;
            `v_out = 0;
            `n_out = d[15];
            `z_out = (d == 0);
        end
        `MOVB: begin
            d = { b[15:8], a[7:0] };
            `v_out = 0;
            `n_out = d[7];
            `z_out = (d[7:0] == 0);
        end
        `CMP: begin		// d = a - b (no register store)
            { `c_out, d } = { 1'd1, a } - b;
            `v_out = (a[15] != b[15]) && (d[15] == b[15]);
            `n_out = d[15];
            `z_out = (d == 0);
        end
        `CMPB: begin
            { d[15:8], `c_out, d[7:0] } = { b[15:8],
                    ({ 1'd1, a[7:0] } - b[7:0]) };
            `v_out = (a[7] != b[7]) && (d[7] == b[7]);
            `n_out = d[7];
            `z_out = (d[7:0] == 0);
        end
        `ADD: begin		// d = a + b
            { `c_out, d } = a + b;
            `v_out = (a[15] == b[15]) && (d[15] != a[15]);
            `n_out = d[15];
            `z_out = (d == 0);
        end
        `SUB: begin		// d = b - a
            { `c_out, d } = {1'd1, b} - a;
            `v_out = (a[15] != b[15]) && (d[15] == a[15]);
            `n_out = d[15];
            `z_out = (d == 0);
        end
        `ASH: begin		// d = a>0 ? b<<a | b>>(-a)
            if (a[5])
                    { `c_out, d } = { 1'd0, b } << (5'd1 + ~a[4:0]);
            else
                    { d, `c_out } = { b, 1'd0 } >> a[4:0];
            `v_out = b[15] ^ d[15];
            `n_out = d[15];
            `z_out = (d == 0);
        end
        `BIT: begin		// d = a & b (no register store)
            d = a & b;
            `v_out = 0;
            `n_out = d[15];
            `z_out = (d == 0);
        end
        `BITB: begin
            d = { b[15:8], (a[7:0] & b[7:0]) };
            `v_out = 0;
            `n_out = d[7];
            `z_out = (d[7:0] == 0);
        end
        `BIC: begin		// d = ~a & b
            d = ~a & b;
            `v_out = 0;
            `n_out = d[15];
            `z_out = (d == 0);
        end
        `BICB: begin
            d = { b[15:8], (~a[7:0] & b[7:0]) };
            `v_out = 0;
            `n_out = d[7];
            `z_out = (d[7:0] == 0);
        end
        `BIS: begin		// d = a | b
            d = a | b;
            `v_out = 0;
            `n_out = d[15];
            `z_out = (d == 0);
        end
        `BISB: begin
            d = { b[15:8], (a[7:0] | b[7:0]) };
            `v_out = 0;
            `n_out = d[7];
            `z_out = (d[7:0] == 0);
        end
        `XOR: begin		// d = a ^ b
            d = a ^ b;
            `v_out = 0;
            `n_out = d[15];
            `z_out = (d == 0);
        end

        //
        // Branch instructions.
        //
        `JMP: begin		// d = a
            d = a;
        end
        `JMP2: begin		// d = a + b
            d = a + b;
        end
        `BR: begin              // d = (a << 1) + b (nonstandard)
            d = b + offset;
        end
        `BNE, `BEQ: begin	// z
            d = b + (`z_in == op[2] ? offset : 0);
        end
        `BGE, `BLT: begin	// n^v
            d = b + ((`n_in ^ `v_in) == op[2] ? offset : 0);
        end
        `BGT, `BLE: begin	// z | n^v
            d = b + ((`z_in | (`n_in ^ `v_in)) == op[2] ? offset : 0);
        end
        `BPL, `BMI: begin	// n
            d = b + (`n_in == op[2] ? offset : 0);
        end
        `BHI, `BLOS: begin	// c | z
            d = b + ((`c_in | `z_in) == op[2] ? offset : 0);
        end
        `BVC, `BVS: begin	// v
            d = b + (`v_in == op[2] ? offset : 0);
        end
        `BHIS, `BLO: begin	// c
            d = b + (`c_in == op[2] ? offset : 0);
        end
        endcase
    end
endmodule
