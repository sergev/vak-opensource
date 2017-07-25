module top (
    input  wire [9:0] switch,
    output wire [9:0] light
);
    assign light = switch;

endmodule
