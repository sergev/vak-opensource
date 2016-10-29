module rom8kx16 (
	input [12 : 0] addr,
	input clk,
	output [15 : 0] dout);

	//
	// Generate 8 blocks of memory 8k x 2.
	// Initialize memory contents from rom-init-*.v files.
	//
	RAMB16_S2 #(.INIT (2'b00), .SRVAL (2'b00), .WRITE_MODE ("WRITE_FIRST"),
		`include "rom-init-a.v"
	) rom_1_0 (.DO (dout[1:0]), .ADDR (addr), .CLK (clk),
		.DI (0), .EN (1), .SSR (0), .WE (0));

	RAMB16_S2 #(.INIT (2'b00), .SRVAL (2'b00), .WRITE_MODE ("WRITE_FIRST"),
		`include "rom-init-b.v"
	) rom_3_2 (.DO (dout[3:2]), .ADDR (addr), .CLK (clk),
		.DI (0), .EN (1), .SSR (0), .WE (0));

	RAMB16_S2 #(.INIT (2'b00), .SRVAL (2'b00), .WRITE_MODE ("WRITE_FIRST"),
		`include "rom-init-c.v"
	) rom_5_4 (.DO (dout[5:4]), .ADDR (addr), .CLK (clk),
		.DI (0), .EN (1), .SSR (0), .WE (0));

	RAMB16_S2 #(.INIT (2'b00), .SRVAL (2'b00), .WRITE_MODE ("WRITE_FIRST"),
		`include "rom-init-d.v"
	) rom_7_6 (.DO (dout[7:6]), .ADDR (addr), .CLK (clk),
		.DI (0), .EN (1), .SSR (0), .WE (0));

	RAMB16_S2 #(.INIT (2'b00), .SRVAL (2'b00), .WRITE_MODE ("WRITE_FIRST"),
		`include "rom-init-e.v"
	) rom_9_8 (.DO (dout[9:8]), .ADDR (addr), .CLK (clk),
		.DI (0), .EN (1), .SSR (0), .WE (0));

	RAMB16_S2 #(.INIT (2'b00), .SRVAL (2'b00), .WRITE_MODE ("WRITE_FIRST"),
		`include "rom-init-f.v"
	) rom_11_10 (.DO (dout[11:10]), .ADDR (addr), .CLK (clk),
		.DI (0), .EN (1), .SSR (0), .WE (0));

	RAMB16_S2 #(.INIT (2'b00), .SRVAL (2'b00), .WRITE_MODE ("WRITE_FIRST"),
		`include "rom-init-g.v"
	) rom_13_12 (.DO (dout[13:12]), .ADDR (addr), .CLK (clk),
		.DI (0), .EN (1), .SSR (0), .WE (0));

	RAMB16_S2 #(.INIT (2'b00), .SRVAL (2'b00), .WRITE_MODE ("WRITE_FIRST"),
		`include "rom-init-h.v"
	) rom_15_14 (.DO (dout[15:14]), .ADDR (addr), .CLK (clk),
		.DI (0), .EN (1), .SSR (0), .WE (0));
endmodule
