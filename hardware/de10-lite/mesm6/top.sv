`default_nettype none

module top (
    input  wire       MAX10_CLK1_50,
    input  wire [9:0] sw,
    output reg [35:0] gpio
);
    reg  [47:0] acc;
    reg  [47:0] mask;
    wire [47:0] result = func(acc, mask);

    wire clk = MAX10_CLK1_50;

    reg [35:0] count = '0;

    always @(clk) begin
        count <= count + 1;

        if (count[35]) gpio[23:0] <= result[23:0];
        else           gpio[23:0] <= result[47:24];

        if (count[34])      acc[9:0]   <= sw;
        else if (count[33]) acc[19:10] <= sw;
        else if (count[32]) acc[29:20] <= sw;
        else if (count[31]) acc[39:30] <= sw;
        else                acc[47:40] <= sw[7:0];

        if (count[30])      mask[9:0]   <= sw;
        else if (count[29]) mask[19:10] <= sw;
        else if (count[28]) mask[29:20] <= sw;
        else if (count[27]) mask[39:30] <= sw;
        else                mask[47:40] <= sw[7:0];

    end

    function [47:0] func(
        input wire [47:0] val,
        input wire [47:0] mask
    );
        reg [47:0] result;
`ifdef notdef
        // Pack
        int i;

        result = '0;
        for (i=0; i<48; i++) begin
            if (mask[i])
                result = { val[i], result[47:1] };
        end
`endif

//`ifdef notdef
        // Unpack
        int i, k;

        result = '0;
        k = 47;
        for (i=47; i>=0; i--) begin
            if (mask[i]) begin
                result[i] = val[k];
                k--;
            end
        end
//`endif

        //result = val & mask;
        return result;
    endfunction
endmodule
