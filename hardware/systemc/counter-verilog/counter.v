`timescale 1ns / 1ns
//
// Negate-or function.
//

module counter_m (
    input wire clock,
    input wire reset,
    input wire enable,
    output reg [3:0] counter_out
);

    always @(posedge clock) begin
        if (reset)
            counter_out <= 0;

        else if (enable) begin
            $display("(%0d ns) Incremented Counter %d", $time, counter_out);
            counter_out <= counter_out + 1;
        end
    end
endmodule
