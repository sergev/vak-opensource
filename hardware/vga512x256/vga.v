//
// Videoadapter 512x256 one bit per pixel.
// Clock 25 MHz.
// Uses 16 kbytes of video memory: 8192 words of 16 bit each
//
module vga512x256 (
	input wire clk,
	input wire rst,
	output wire [12:0] maddr,	// address to video memory
	input wire [15:0] mdata,	// data from video memory
	output wire red,
	output wire green,
	output wire blue,
	output wire hsync,
	output wire vsync);

	//
	// Horizontal timing
	//          ______________________          ________
	// ________|        VIDEO         |________| VIDEO (next line)
	//     |-C-|----------D-----------|-E-|
	// __   ______________________________   ___________
	//   |_|                              |_|
	//   |B|
	//
	parameter B = 50;	// Sync pulse length
	parameter C = 92;	// Back porch
	parameter D = 512;	// Active video
	parameter E = 36;	// Front porch

	//
	// Vertical timing
	//          ______________________          ________
	// ________|        VIDEO         |________|  VIDEO (next frame)
	//     |-Q-|----------R-----------|-S-|
	// __   ______________________________   ___________
	//   |_|                              |_|
	//   |P|
	//
	parameter P = 4;	// Sync length
	parameter Q = 61;	// Back porch
	parameter R = 512;	// Active video
	parameter S = 31;	// Front porch

	// Current pixel's X-Y position.
	reg [9:0] cnt_x;
	reg [9:0] cnt_y;

	// Video data shift register.
	reg [15:0] word;

	// Video output bits.
	assign red = word[0];
	assign green = word[0];
	assign blue = word[0];

	// Address of video memory.
	assign maddr = { cnt_y[8:1], cnt_x[8:4] };

	// Delay hsync by 1 clock.
	assign hsync = ~(cnt_x > (D + E) && cnt_x <= (D + E + B));
	assign vsync = ~(cnt_y >= (R + S) && cnt_y < (R + S + P));

	always @(posedge clk) begin
		// Horizontal dot counter.
		if (rst || cnt_x == (B + C + D + E - 1))
			cnt_x <= 0;
		else
			cnt_x <= cnt_x + 1;

		// Vertical line counter.
		if (rst)
			cnt_y <= 0;
		else if (cnt_x == (B + C + D + E - 1)) begin
			if (cnt_y == (P + Q + R + S - 1))
				cnt_y <= 0;
			else
				cnt_y <= cnt_y + 1;
		end

		// Video data shift register.
		// Reading from memory must be delayed by 1 clock.
		if (cnt_x[3:0] == 1) begin
			if (cnt_x <= D && cnt_y < R)
				word <= mdata;
			else
				word <= 0;
		end else
			word <= { 1'b0, word[15:1] };
	end
endmodule
