`timescale 1ns / 1ps
//
// Behavioral RAM.
//
module memory (
    input wire [15:0] addr,	// address bus
    input wire we,		// write enable
    input wire clk,		// write clock
    input wire bytew,		// byte write
    input wire [15:0] d_in,	// write data bus
    output wire [15:0] d_out	// read data bus
);
    // 64 kbytes of memory
    reg [15:0] memory [64*512-1:0];

    // Addressed word
    wire [15:0] word = memory [addr[15:1]];

    // Read memory
    assign d_out = addr[0] ?
        (word >> 8) :		// byte read
        word;			// word-aligned read

    // Write memory on positive clk, when we is set.
    always @(posedge clk) begin
        if (we == 1) begin
            if (bytew) begin
                // byte write
                if (addr[1]) begin	// high byte
                    memory [addr[15:1]] = { d_in[7:0], word[7:0] };
                    $display ("(%0d) store %h := %h", $time, {addr[15:1], 1'b0}, { d_in[7:0], word[7:0] });
                end else begin		// low byte
                    memory [addr[15:1]] = { word[15:8], d_in[7:0] };
                    $display ("(%0d) store %h := %h", $time, {addr[15:1], 1'b0}, { word[15:8], d_in[7:0] });
                end
            end else begin
                // word write
                memory [addr[15:1]] = d_in;
                $display ("(%0d) store %h := %h", $time, {addr[15:1], 1'b0}, d_in);
            end
        end
    end

//    initial begin
//        // Initialize RAM contents on start.
//        `include "memory-initial.v"
//    end
endmodule
