`timescale 1ns / 1ps
//
// PDP-11 datapath.
//
module datapath (
    input wire clk,             // External clock signal
    input wire reset            // External active high reset signal
);
    // Control.
    wire [2:0] ctl_reg_src, ctl_reg_dst; // src/dst reg numbers
    wire [1:0] ctl_alu_input;   // alu source from src or x, dst or y
    wire [9:0] ctl_alu_op;	// alu operation code
    wire [2:0] ctl_mem_addr;    // memory address src/dst/src+x/dst+y/x/y/z
    wire ctl_mem_byte;          // memory byte mode
    wire ctl_reg_input;         // write to regfile from mem out
    wire ctl_reg_we;            // register file write enable
    wire ctl_x_we;              // x register write
    wire ctl_y_we;              // y register write
    wire ctl_z_we;              // z register write
    wire ctl_psw_we;            // psw register write
    wire ctl_mem_we;            // memory write
    wire ctl_ir_we;             // instruction register write

    // Output of ALU
    wire [15:0] alu_out;

    // Output of RAM
    wire [15:0] mem_out;

    // Output of register file: source and destination registers
    wire [15:0] src, dst;

    // Input of register file: from memory or from ALU
    wire [15:0] reg_input = ctl_reg_input ? mem_out : alu_out;

    // Register file.
    // On reset, PC is cleared.
    regfile regfile (
        .sela	(ctl_reg_src),
        .selb	(ctl_reg_dst),
        .we	(ctl_reg_we),
        .clk	(clk),
        .reset	(reset),
        .w	(reg_input),
        .a	(src),
        .b	(dst)
    );

    // Registers X, Y - hold data from memory.
    // Used for source and destination indirect addressing modes.
    reg [15:0] x, y;
    always @(posedge clk) begin
        if (ctl_x_we == 1) begin
            x <= mem_out;
            $display ("%4d-%1d) X := %o", $time, cycount, mem_out);
        end
        if (ctl_y_we == 1) begin
            y <= mem_out;
            $display ("%4d-%1d) Y := %o", $time, cycount, mem_out);
        end
    end

    // Data buses on input of ALU
    wire [15:0] alu_ina = ctl_alu_input[0] ? x : src;
    wire [15:0] alu_inb = ctl_alu_input[1] ? y : dst;

    // Resulting state
    wire [7:0] alu_state;

    // Register PSW - hold ALU state
    reg [7:0] psw;
    always @(posedge clk) begin
        if (ctl_psw_we == 1) begin
            psw <= alu_state;
            $display ("%4d-%1d) PSW := %02o", $time, cycount, alu_state);
        end
    end

    // ALU
    alu alu (
        .op	(ctl_alu_op),
        .a	(alu_ina),
        .b	(alu_inb),
        .ps_in	(psw),
        .d	(alu_out),
        .ps_out	(alu_state)
    );

    // Memory address
    wire [15:0] mem_addr =
        (ctl_mem_addr == 3'd0) ? src :		// src
        (ctl_mem_addr == 3'd1) ? src + x :	// src + X
        (ctl_mem_addr == 3'd2) ? dst :		// dst
        (ctl_mem_addr == 3'd3) ? dst + y :	// dst + Y
        (ctl_mem_addr == 3'd4) ? x :		// X
        (ctl_mem_addr == 3'd5) ? y :		// Y
                                 z;		// Z

    // Register Z - hold memory address
    reg [15:0] z;
    always @(posedge clk) begin
        if (ctl_z_we == 1) begin
            z <= mem_addr;
            $display ("%4d-%1d) Z := %o", $time, cycount, mem_addr);
        end
    end

    // RAM
    memory ram (
        .addr	(mem_addr),
        .we	(ctl_mem_we),
        .clk	(clk),
        .bytew	(ctl_mem_byte),
        .d_in	(alu_out),
        .d_out	(mem_out)
    );


    // Instruction register.
    reg [15:0] ir;
    always @(posedge clk) begin
        if (ctl_ir_we == 1 && ! reset) begin
            ir <= mem_out;
            $c ("extern void trace_fetch (unsigned cycount, unsigned mem_addr, unsigned mem_out);");
            $c ("trace_fetch (", cycount, ",", mem_addr, ",", mem_out, ");");
            //$display ("        reg_src %o - src %o", ctl_reg_src, src);
            //$display ("        reg_dst %o - dst %o", ctl_reg_dst, dst);
        end
    end

    // Cycle counter. Changed on negative clk edge.
    // On reset, TC is cleared.
    reg [2:0] cycount;
    wire [2:0] cycount_next;
    always @(posedge clk) begin
        //$display ("%4d-%1d) cycount := %0d", $time, cycount, reset ? 0 : cycount_next);
        cycount <= reset ? 0 : cycount_next;
    end

    // Control unit
    control control (
        .cmd		(ir),
        .cycle		(cycount),
        .cnext		(cycount_next),
        .reg_from_mem	(ctl_reg_input),
        .reg_src	(ctl_reg_src),
        .reg_dst	(ctl_reg_dst),
        .reg_we		(ctl_reg_we),
        .mem_addr	(ctl_mem_addr),
        .mem_byte	(ctl_mem_byte),
        .x_we		(ctl_x_we),
        .y_we		(ctl_y_we),
        .z_we		(ctl_z_we),
        .alu_input	(ctl_alu_input),
        .alu_op		(ctl_alu_op),
        .psw_we		(ctl_psw_we),
        .mem_we		(ctl_mem_we),
        .ir_we		(ctl_ir_we)
    );

endmodule
