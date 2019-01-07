module testbench();

    // External C function.
    import "DPI-C" function void c_func();

initial begin
    $display("----- Start -----");
    c_func();
    $display("----- Finish -----");
    $finish;
end

endmodule
