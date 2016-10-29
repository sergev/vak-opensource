/*
 * Example of simple simulation.
 */
`timescale 1ns / 1ns

module main;

    /*
     * Signals.
     */
    reg       clock;    /* Clock input of the design */
    reg       reset;    /* Active high, synchronous Reset */
    reg       enable;   /* Active high enable signal for counter */
    reg [3:0] count;    /* 4-bit counter */

    /*
     * Clock generator.
     */
    always begin
        clock <= 0;
        #10;
        clock <= 1;
        #10;
    end

    /*
     * 4-bit up-counter with synchronous active high reset and
     * with active high enable signal.
     */
    always @(posedge clock) begin
        if (reset)
            count <= 0;

        else if (enable) begin
            $display("(%0d) Incremented Counter %d", $time, count);
            count <= count + 1;
        end
    end

    initial begin
        #100;
        reset <= 1;
        $display ("(%0d) Asserting Reset", $time);

        #200;
        reset <= 0;
        $display ("(%0d) De-Asserting Reset", $time);

        #100;
        $display ("(%0d) Asserting Enable", $time);
        enable <= 1;

        #400;
        $display ("(%0d) De-Asserting Enable", $time);
        enable <= 0;

        $display ("(%0d) Terminating simulation", $time);
        $finish;
    end
endmodule
