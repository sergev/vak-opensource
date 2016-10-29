//
// VGA toplevel module.
//
// Copyright (C) 2006 Serge Vakulenko
//
module toplevel (
	input wire clk_50mhz,			// system clock
	input wire btn_south,			// reset button
        output wire vga_red,
        output wire vga_green,
        output wire vga_blue,
        output wire vga_hsync,
        output wire vga_vsync);

	reg clk;				// system clock 25 MHz
	wire rst = btn_south;			// system reset

	wire [12:0] maddr;			// address to video memory
	wire [15:0] mdata;			// data from video memory

	vga512x256 vga (.clk (clk), .rst (rst),
		.maddr (maddr), .mdata (mdata),
		.red (vga_red), .green (vga_green), .blue (vga_blue),
		.hsync (vga_hsync), .vsync (vga_vsync));

	rom8kx16 rom (.clk (clk), .addr (maddr), .dout (mdata));

	// Generate clock 25 MHz.
	always @(posedge clk_50mhz) begin
		if (rst)
			clk <= 0;
		else
			clk <= ~clk;
	end

endmodule
