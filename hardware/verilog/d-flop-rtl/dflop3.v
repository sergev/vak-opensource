//
// D-type flip-flop from Wiki page.
// Note 3-input gate with output M.
//
`timescale 1ns / 1ns

module main;

    //
    // Signals.
    //
    reg     cp;         // Clock input
    reg     d;          // Data input

    wire    q;          // Flop output
    wire    nq;         // Negated flop output
    wire    k, l, m, n; // Four intermediate NOR outputs

    //
    // Clock generator.
    //
    initial begin
        cp <= 0;

        #10; cp <= 1; #10; cp <= 0;
        #10; cp <= 1; #10; cp <= 0;
        #10; cp <= 1; #10; cp <= 0;
        #10; cp <= 1; #10; cp <= 0;
        #10; cp <= 1; #10; cp <= 0;
        #10; cp <= 1; #10; cp <= 0;

        #40; cp <= 1; #40; cp <= 0;
    end

    //
    // Six NOR gates.
    //
    assign #1 k = ~(l | n);
    assign #1 l = ~(k | cp);
    assign #1 m = ~(l | cp | n); // Note: three inputs
    assign #1 n = ~(m | d);
    assign #1 q = ~(l | nq);
    assign #1 nq = ~(m | q);

    initial begin
        $dumpfile("dflop3.vcd");
        $dumpvars(0, main);

        d <= 0;
        #45;
        d <= 1;
        $display ("(%0d) Asserting Data", $time);

        #40;
        d <= 0;
        $display ("(%0d) De-Asserting Data", $time);

        #40; d <= 1; #10; d <= 0;
        #10; d <= 1; #10; d <= 0;
        #10; d <= 1; #10; d <= 0;
        #10; d <= 1; #10; d <= 0;

        #20;
        $display ("(%0d) Terminating simulation", $time);
        $finish;
    end
endmodule
