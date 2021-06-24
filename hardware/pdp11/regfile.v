`timescale 1ns / 1ps
//
// Register file.
//
module regfile (
    input wire [2:0] sela,
    input wire [2:0] selb,
    input wire we,
    input wire clk,
    input wire reset,
    input wire [15:0] w,
    output wire [15:0] a,
    output wire [15:0] b
);
    reg [15:0] r [7:0];

    // Output R[a] on bus A
    assign a = r [sela];

    // Output R[b] on bus B
    assign b = r [selb];

    // Write value from bus W to R[b].
    // On positive clk, when we is set.
    always @(posedge clk) begin
        $c ("extern void trace_reg (unsigned cycount, unsigned reg, unsigned val);");
        if (reset) begin
            // On reset, PC is cleared.
            r [7] = 0;
            //$c ("trace_reg (", datapath.cycount, ",", 7, ",", 0, ");");
        end else if (we == 1) begin
            r [selb] = w;
            //$c ("trace_reg (", datapath.cycount, ",", selb, ",", w, ");");
        end
    end

endmodule
